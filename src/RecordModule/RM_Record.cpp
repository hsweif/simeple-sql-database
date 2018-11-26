#include "../include/RecordModule/RM_Record.h"
#include <iostream>
using namespace std;

RM_node::RM_node(BufType buf, int l, int t)
{
    length = l;
    ctx = buf;
    type = t;
}

void RM_node::setCtx(int n)
{
    length = 1;
    ctx = new uint;
    ctx[0] = (uint) n;
}

void RM_node::setCtx(float f)
{
    length = 1;
    ctx = new uint;
    ctx[0] = (uint) f;
}

void RM_node::setCtx(string s)
{
    int l = 0;
    int strLength = s.length();
    type = STR_TYPE;
    if(strLength < ITEM_LENGTH)
    {
        l = ITEM_LENGTH;
    }
    else if(strLength < DESCRIPT_LENGTH)
    {
        l = DESCRIPT_LENGTH;
    }
    else
    {
        cout << "[Error] item should not be more than 256 char" << endl;
        return;
    }
    length = l;
    ctx = new uint[l/4];
    for(int i = 0; i < l/4; i ++) {
        ctx[i] = 0;
    }
    int cnt = 0;
    int offset = 0;
    for(int i = 0; i < l; i ++)
    {
        uint c = 0;
        if(i < strLength) {
            c = (uint)s[i] << offset;
        }
        ctx[cnt] += c;
        if(offset != 24)
        {
            offset += 8;
        }
        else
        {
            cnt ++;
            offset = 0;
        }
    }
}

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

RM_Record::RM_Record(): recordSize(-1),mData(NULL)
{
	RID id(-1,-1);
	mRid = id;
}

RM_Record::RM_Record(vector<int> _type)
{
    type = _type;
    length = _type.size();
}

int RM_Record::SetRecord(BufType pData,int size,RID id){
	mData = pData;
	recordSize = size;
	mRid = id;
	return 0;
}

void RM_Record::SetType(vector<int> tp)
{
    this->type = tp;
}

BufType RM_Record::GetData() const
{
	if (recordSize == -1 || mData == NULL)
		return NULL;
	return mData;
}

int RM_Record::GetRid(RID &id) const
{
	if(recordSize == -1 || mData == NULL)
		return 1;
	id = mRid;
	return 0;	
}

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
    if(this->GetNodes(result, this->GetData(), recordSize)) {
        cout << "error to get Record" << endl;
    }
    for(int i = 0; i < result.size(); i ++) {
        result[i].Print();
        cout << "|";
    }
    cout << endl;
}


int RM_Record::GetSerializeRecord(BufType *rec, vector<RM_node> data, int &recordSize)
{
    if (data.size() != type.size())
    {
        return 1;
    }
    int size = 0;
    int data_l = data.size();
    for (int i = 0; i < data_l; i++)
    {
        if (type[i] == FLOAT_TYPE || type[i] == INT_TYPE)
        {
            size += 1;
            data[i].length = 1;
        }
        else if (type[i] == STR_TYPE)
        {
            int sLength = ITEM_LENGTH / 4;
            size += sLength;
            data[i].length = sLength;
        }
        else if (type[i] == DESCRIPTION)
        {
            int dLength = DESCRIPT_LENGTH / 4;
            size += dLength;
            data[i].length = dLength;
        }
    }
    recordSize = size;
    recordLength = size;
    BufType buf = new uint[recordSize];
    int cnt = 0;
    for(int i = 0; i < data_l; i ++)
    {
        int node_l = data[i].length;
        for(int j = 0; j < node_l; j ++)
        {
            buf[cnt] = (uint)data[i].ctx[j];
            cnt ++;
        }
    }
    *rec = buf;
    return 0;
}


int RM_Record::GetNodes(vector<RM_node> &result, BufType serializedBuf, int bufLength)
{
    vector<RM_node> vec;
    int cnt = 0;
    int l = type.size();
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

