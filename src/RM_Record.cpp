#include "../include/RecordModule/RM_Record.h"
#include <iostream>
using namespace std;
RM_Record::RM_Record()
:recordSize(-1),mData(NULL)
{
	RID id(-1,-1);
	mRid = id;
}

int RM_Record::SetRecord(char *pData,int size,RID id){
	mData = pData;
	recordSize = size;
	mRid = id;
	return 0;
}

int RM_Record::GetData(char *&pData) const
{
	if(recordSize == -1 || mData == NULL)
		return 1;
	pData = mData;
	return 0;
}

int RM_Record::GetRid(RID &id) const
{
	if(recordSize == -1 || mData == NULL)
		return 1;
	id = mRid;
	return 0;	
}
