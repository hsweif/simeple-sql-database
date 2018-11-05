#include "../include/RecordModule/RM_FileHandle.h"
int RM_FileHandle::GetRec(const RID &rid, RM_Record &rec) const{
	int page;
	int slot;
	if(rid.GetPageNum(page)>0||rid.GetSlotNum(slot)>0)
		return 1;

	return 0;
}

int RM_FileHandle::InsertRec(const char *pData, RID &rid){
	return 0;
}

int RM_FileHandle::DeleteRec(){
	return 0;
}

int RM_FileHandle::UpdateRec(){
	return 0;
}

int RM_FileHandle::ForcePages(){
	return 0;
}
