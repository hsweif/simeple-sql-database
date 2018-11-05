#include "../include/RecordModule/RM_FileHandle.h"
int RM_FileHandle::GetRec(const RID &rid, RM_Record &rec) const{
	int page, slot;
	if(rid.GetPageNum(page) > 0 || rid.GetSlotNum(slot) > 0) {
		return 1;
	}
	BufType buf = new uint[PAGE_INT_NUM];
	BufType result = new uint[this->recordSize];
	FileManager *fm = new FileManager();
	fm->readPage(this->fileId, page, buf, 0);
	result = &buf[slot * this->recordSize];
	rec.SetRecord(result, this->recordSize, rid);
	return 0;
}

int RM_FileHandle::InsertRec(const RM_Record& pData, RID &rid){
	FileManager *fm = new FileManager();
	int page, slot;
	if(rid.GetPageNum(page) > 0 || rid.GetSlotNum(slot) > 0) {
		return 1;
	}
	BufType buf = new uint[PAGE_INT_NUM], data;
	fm->readPage(this->fileId, page, buf, 0);
	int rSize;
	if(pData.getSize(rSize) || pData.GetData(data)) {
		return 1;
	}
	for(int i = 0; i < rSize; i ++) {
		buf[slot * rSize + i] = data[i];
	}
	fm->writePage(this->fileId, page, buf, 0);
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
