#ifndef RM_RECORD_H
#define RM_RECORD_H
#include "RID.h"
#include "../utils/pagedef.h"
#include <vector>
#include <string>
#include <string.h>
#include <iostream>
#include <cstdio>

#define INT_TYPE 0
#define STR_TYPE 1
#define FLOAT_TYPE 2
#define ERR_TYPE -1
#define DESCRIPTION 3
#define ITEM_LENGTH 16
#define DESCRIPT_LENGTH 256

using namespace std;
struct RM_node
{
    int length;
    int type;
    BufType ctx; //Context of the node
    RM_node(){}
    RM_node(BufType buf, int l, int t = STR_TYPE);
    void setCtx(int n);
    void setCtx(float f);
    void setCtx(string s);
    void Print();
};

class RM_Record {
private:
	RID mRid;
	int recordSize;
	int length;
	BufType mData;
	int recordLength;
	vector<int> type;
public:
	RM_Record  ();                     // Constructor
	RM_Record(vector<int> type);
	~RM_Record (){};                     // Destructor
	int SetRecord	(BufType pData,int size,RID id);
	void SetType(vector<int> tp);
	BufType GetData () const;   // Set pData to point to
	//   the record's contents
	int GetRid     (RID &rid) const;       // Get the record id
	int GetSize(int &sz) const;
	int GetSerializeRecord(BufType *rec, vector<RM_node> data, int &recordSize);
	int GetNodes(vector<RM_node> &result, BufType serializedBuf);
	int GetColumn(int col, string *content);
	void Print();
};
#endif