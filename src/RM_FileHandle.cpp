#include "../include/RecordModule/RM_FileHandle.h"
int RM_FileHandle::GetRec(const RID &rid, RM_Record &rec) const{
	int page;
	int slot;
	if(rid.GetPageNum(page)>0||rid.GetSlotNum(slot)>0)
		return 1;

	return 0;
}

int RM_FileHandle::init(int _fileId, int _recordSize, int _recordPP, int _recordSum, int _pageCnt, BufType _pageMap) 
{
	fileId = _fileId;
	recordSize = _recordSize;
	recordPP = _recordPP;
	recordSum = _recordSum;
	pageCnt = _pageCnt;
	pageMap = _pageMap;
}

int RM_FileHandle::InsertRec(const char *pData, RID &rid){
	return 0;
}

int RM_FileHandle::DeleteRec(const RID &rid){
	return 0;
}

int RM_FileHandle::UpdateRec(const RM_Record &rec){
	return 0;
}


