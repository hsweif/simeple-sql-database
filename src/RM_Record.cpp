#include "../include/RecordModule/RM_Record.h"
#include <iostream>
using namespace std;
RM_Record::RM_Record()
:recordSize(-1),mData(NULL)
{
	RID id(-1,-1);
	mRid = id;
}

int RM_Record::SetRecord(BufType pData,int size,RID id){
	mData = pData;
	recordSize = size;
	mRid = id;
	return 0;
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
	return 0;
}