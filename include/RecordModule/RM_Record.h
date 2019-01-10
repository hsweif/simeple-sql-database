#ifndef RM_RECORD_H
#define RM_RECORD_H
#include "RID.h"
#include "../utils/pagedef.h"
#include <vector>
#include <string>
#include <string.h>
#include <iostream>
#include <cstdio>
#include <sstream>
#include <config.h>
#include <algorithm>

using namespace std;

namespace RM {
	enum ItemType {
		CHAR, INT, FLOAT, ERROR
	};
	int float2Uint(float a, uint *ptr);
	uint castFloatToUint(float f);
	float castUintToFloat(unsigned int i);
}



struct RM_node
{
    int length;
    RM::ItemType type;
    int isNull;

    int num;
    string str;
    float fNum;

    BufType ctx; //Context of the node
    RM_node();
    ~RM_node() {
    	// if(ctx != nullptr)	{
    	// 	if(type == RM::CHAR) {
    	// 		delete [] ctx;
		// 	}
		// }
	}
    RM_node(int content);
	RM_node(float content);
	RM_node(string content);
    void setCtx(int n);
    void setCtx(float f);
    void setCtx(string s);
    BufType getCtx();
    bool CmpCtx(IM::CompOp compOp, string value);
    bool operator == (const RM_node &b);
};

class RM_Record {
private:
	RID mRid;
	int recordSize;
	BufType mData;
	int bufSize;
	int colNum;
public:
	RM_Record();
	~RM_Record (){ }//if(mData != NULL) delete [] mData;};
	int SetRecord	(BufType pData, int size, int cNum);
	void SetRecord   (int offset,uint data);
	BufType GetData () const; // 返回不包括NULL位图的内容
	BufType GetBuf () const; // 返回整个mData，包括NULL位图
	//   the record's contents
	int GetRid     (RID &rid) const;       // Get the record id
	int RecordSize() const { return recordSize;	}
	int BufSize() const { return bufSize; }
	bool IsNull(int pos);
	void SetNull(int pos);
	int SetRID(RID &rid) { this->mRid = rid;};
};
#endif