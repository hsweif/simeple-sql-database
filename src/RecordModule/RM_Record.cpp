#include "../include/RecordModule/RM_Record.h"
#include <iostream>
#include "config.h"

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

bool RM_node::operator == (const RM_node &b)
{
    if(this->type != b.type || this->length != b.length)   {
        return false;
    }
    if(this->type == RM::ItemType::CHAR && this->str != b.str) {
        return false;
    }
    else if(this->type == RM::ItemType::FLOAT && this->fNum != b.fNum) {
        return false;
    }
    else if(this->type == RM::ItemType::INT && this->num != b.num) {
        return false;
    }
    return true;
}

void RM_node::setCtx(int n)
{
    type = RM::INT;
    length = 1;
    num = n;
    fNum = (float)n;
    ctx = new uint;
    ctx[0] = (uint) n;
}

void RM_node::setCtx(float f)
{
    type = RM::FLOAT;
    length = 1;
    fNum = f;
    num = (int)f;
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
    memset(ctx, 0, sizeof(uint)*length);
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
    std::transform(value.begin(), value.end(), value.begin(), ::toupper);
    std::transform(mValue.begin(), mValue.end(), mValue.begin(), ::toupper);
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
}