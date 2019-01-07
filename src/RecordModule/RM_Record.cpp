#include "../include/RecordModule/RM_Record.h"
#include <iostream>
#include <config.h>

using namespace std;

namespace RM{
    int float2Uint(float a, uint *ptr)
    {
        unsigned int *p = (unsigned int *)&a;
        float b;
        memcpy(&b,p,sizeof(float));
    }
    uint castFloatToUint(float f)
    {
        union
        {
            float f;
            unsigned int i;
        } u;
        u.f = f;
        return u.i;
    }
    float castUintToFloat(unsigned int i)
    {
        union
        {
            float f;
            unsigned int i;
        } u;
        u.i = i;
        return u.f;
    }
}

RM_node::RM_node()
{
    isNull = true;
}

RM_node::RM_node(int content)
{
    isNull = false;
    setCtx(content);
}

RM_node::RM_node(float content)
{
    isNull = false;
    setCtx(content);
}

RM_node::RM_node(string content)
{
    isNull = false;
    setCtx(content);
}

void RM_node::setCtx(int n)
{
    type = RM::INT;
    length = 1;
    num = n;
    ctx = new uint;
    ctx[0] = (uint) n;
}

void RM_node::setCtx(float f)
{
    type = RM::FLOAT;
    length = 1;
    fNum = f;
    ctx = new uint;
    // RM::float2Uint(f, ctx);
    // ctx[0] = f;
    ctx[0] = RM::castFloatToUint(f);
}

void RM_node::setCtx(string s)
{
    str = s;
    type = RM::CHAR;
    int strLength = s.length();
    length = s.length();
    ctx = new uint[length];
    memset(ctx, 0, sizeof(ctx));
    int cnt = 0;
    int offset = 0;
    for(int i = 0; i < strLength; i ++)
    {
        uint c = (uint)s[i] << offset;
        ctx[cnt] += c;
        if(offset != 24) {
            offset += 8;
        }
        else {
            cnt ++;
            offset = 0;
        }
    }
}

BufType RM_node::getCtx(){
    return ctx;
}
bool RM_node::CmpCtx(IM::CompOp compOp, string value)
{
    std::stringstream ss;
    string mValue;
    if(type == RM::INT) {
        ss << num;
        ss >> mValue;
    }
    else if(type == RM::FLOAT) {
        ss << fNum;
        ss >> mValue;
    }
    else{
        mValue = str;
    }
    int res = strcmp(value.c_str(), mValue.c_str());
    if(compOp == IM::EQ) {
        return res == 0;
    }
    else if(compOp == IM::LS) {
        return res > 0;
    }
    else if(compOp == IM::LEQ) {
        return res >= 0;
    }
    else if(compOp == IM::GT) {
        return res < 0;
    }
    else if(compOp == IM::GEQ) {
        return res <= 0;
    }
    else if(compOp == IM::NEQ) {
        return res != 0;
    }
    else{
        return false;
    }
}
/*
void RM_node::Print()
{
    if(type == STR_TYPE)
    {
        uint offset = 255;
        uint u;
        int l = ITEM_LENGTH/4;
        for(int k = 0; k < l; k ++)
        {
            u = ctx[k];
            for(int i = 0; i < 4; i ++)
            {
                char c = (char)(( u & (offset << (i*8))) >> (i*8));
                if(c > 31 & c < 127)
                    cout << c;
            }
        }
    }
}
*/

RM_Record::RM_Record(): recordSize(-1),mData(NULL)
{
	RID id(-1,-1);
	mRid = id;
}


int RM_Record::SetRecord(BufType pData, int size, int cNum){
    colNum = cNum;
	if(pData == NULL) {
	    cout << "pData is null" << endl;
	    return 1;
    }
    int offset = (colNum % 32) ? colNum/32 + 1 : colNum/32;
	recordSize = size - offset;
	bufSize = size;
	this->mData = new uint[size];
	for(int i = 0; i < size; i ++) {
	    this->mData[i] = pData[i];
    }
	return 0;
}
void RM_Record::SetRecord(int offset,uint data){
    this->mData[offset] = data;
}
/*
void RM_Record::SetType(vector<int> tp)
{
    this->type = tp;
}
*/

BufType RM_Record::GetData() const
{
	if (recordSize == -1 || mData == NULL) {
        return NULL;
    }
    int offset = (colNum % 32) ? colNum/32 + 1 : colNum/32;
	return mData + offset;
}

BufType RM_Record::GetBuf() const
{
    if (recordSize == -1 || mData == NULL) {
        return NULL;
    }
    return mData;
}

int RM_Record::GetRid(RID &id) const
{
	if(recordSize == -1 || mData == NULL)
		return 1;
	id = mRid;
	return 0;	
}

/*
int RM_Record::GetSize(int &sz) const {
	if(recordSize == -1) {
		return 1;
	}
	sz = recordSize;
	// cout << "get recordSize" << recordSize << ' ' << sz << endl;
	return 0;
}

void RM_Record::Print()
{
    vector<RM_node> result;
    if(this->GetNodes(result, this->GetData())) {
        cout << "error to get Record" << endl;
    }
    for(int i = 0; i < result.size(); i ++) {
        result[i].Print();
        cout << "|";
    }
    cout << endl;
}
*/

/*
int RM_Record::GetSerializeRecord(BufType *rec, vector<RM_node> data, int &recordSize)
{
    int size = 0;
    int data_l = data.size();
    for (int i = 0; i < data_l; i++)
    {
        size += data[i].length;
        // if (data[i].type == RM::FLOAT || data[i].type == RM::INT)
        // {
        //     size += 1;
        //     data[i].length = 1;
        // }
        // else if (data[i].type == RM::CHAR)
        // {
        //     int sLength = ITEM_LENGTH / 4;
        //     size += sLength;
        //     data[i].length = sLength;
        // }
        // else if (data[i].type == DESCRIPTION)
        // {
        //     int dLength = DESCRIPT_LENGTH / 4;
        //     size += dLength;
        //     data[i].length = dLength;

    }
    recordSize = size;
    int l = (data_l % 32) ? data_l/32 + 1 : data_l/32;
    this->bufSize = size + l;
    this->recordSize = size;
    BufType buf = new uint[bufSize];

    // This loop is used for recording null info
    int cnt = 0;
    for(int i = 0; i < l && cnt < data_l; i ++) {
        buf[i] = 0;
        for(int shift = 0; shift < 32 && cnt < data_l; shift ++) {
            buf[i] += (uint)(data[cnt].isNull << shift);
            cnt ++;
        }
    }

    cnt = 0;
    for(int i = 0; i < data_l; i ++)
    {
        if(data[i].type == RM::CHAR)
        {
            int node_l = data[i].length;
            for(int j = 0; j < node_l; j ++)
            {
                buf[cnt+l] = (uint)data[i].ctx[j];
                cnt ++;
            }
        }
        else if(data[i].type == RM::INT || data[i].type == RM::FLOAT)
        {
            buf[cnt+l] = (uint)data[i].ctx[0];
            cnt ++;
        }
    }

    *rec = buf;
    return 0;
}
*/

/*
int RM_Record::GetNodes(vector<RM_node> &result, BufType serializedBuf)
{
    vector<RM_node> vec;
    int cnt = 0;
    int l = (int)type.size();
    for (int i = 0; i < l; i++)
    {
        if(type[i] == INT_TYPE || type[i] == FLOAT_TYPE)
        {
            BufType number = new uint[1];
            number[0] = serializedBuf[cnt];
            vec.push_back(RM_node(number, 1));
            cnt ++;
        }
        else
        {
            int strLength = (type[i] == STR_TYPE) ? ITEM_LENGTH : DESCRIPT_LENGTH;
            int offset = 0;
            uint mask = 255;
            char c[strLength];
            for (int j = 0; j < strLength; j++)
            {
                uint tmp = (serializedBuf[cnt] & (mask << offset)) >> offset;
                c[j] = (char)tmp;
                if (offset == 24)
                {
                    cnt++;
                    offset = 0;
                }
                else
                {
                    offset += 8;
                }
            }
            string str(c);
            RM_node node;
            node.setCtx(str);
            vec.push_back(node);
        }
    }
    result = vec;
    return 0;
}
*/

/*
int RM_Record::GetColumn(int col, string *content)
{
    vector<RM_node> result;
    if(this->mData == NULL) {
        cout << "mData is null, 2" << endl;
    }
    this->GetNodes(result, this->mData);
    if(col >= result.size()) {
        printf("%d %lu\n", col, result.size());
        return 1;
    }
    int strLength = ITEM_LENGTH;
    int offset = 0;
    uint mask = 255;
    char c[strLength];
    BufType context = result[col].ctx;
    int cnt = 0;
    for (int j = 0; j < strLength; j++)
    {
        uint tmp = (context[cnt] & (mask << offset)) >> offset;
        c[j] = (char)tmp;
        if (offset == 24)
        {
            cnt++;
            offset = 0;
        }
        else
        {
            offset += 8;
        }
    }
    string tmp(c);
    *content = tmp;
    return 0;
}
*/

bool RM_Record::IsNull(int pos)
{
    int offset = pos % 32;
    int ret = mData[pos/32];
    bool res = (bool)(((1 << offset) & mData[pos/32]) >> offset);
    return res;
}

void RM_Record::SetNull(int pos){
    int offset = pos % 32;
    int ret = mData[pos/32];
    uint num = 1 << offset;
    num = ~num;
    mData[pos/32] = mData[pos/32] & num;
    printf("num:%u\n",num);
}