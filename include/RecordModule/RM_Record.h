#ifndef RM_RECORD_H
#define RM_RECORD_H
#include "RID.h"
#include "../utils/pagedef.h"
#include <vector>
#include <string>
#include <string.h>
#include <iostream>
#include <cstdio>

using namespace std;

namespace RM {
	enum ItemType {
		CHAR, INT, FLOAT, ERROR
	};
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
    RM_node(int content);
	RM_node(float content);
	RM_node(string content);
    void setCtx(int n);
    void setCtx(float f);
    void setCtx(string s);
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
	~RM_Record (){};
	int SetRecord	(BufType pData, int size, int cNum);
	// void SetType(vector<int> tp);
	BufType GetData () const;   // Set pData to point to
	//   the record's contents
	int GetRid     (RID &rid) const;       // Get the record id
	int RecordSize() const { return recordSize;	}
	int BufSize() const { return bufSize; }
	int GetSerializeRecord(BufType *rec, vector<RM_node> data, int &recordSize);
	// int GetNodes(vector<RM_node> &result, BufType serializedBuf);
	// int GetColumn(int col, string *content);
	bool IsNull(int pos);
	int SetRID(RID &rid) { this->mRid = rid;};
	// void Print();
};
#endif