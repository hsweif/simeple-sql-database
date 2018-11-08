#include "../include/RecordModule/RM_FileHandle.h"
#include <iostream>
using namespace std;
BufType reset;
RM_FileHandle::RM_FileHandle() {
	if (reset == NULL)
		reset = new uint[2048];
	for (int i = 0; i < 2048; i++)
		reset[i] = 0;
}

RM_FileHandle::RM_FileHandle(int id, int sz) {
	this->fileId = id;
	this->recordSize = sz;
}

RID RM_FileHandle::checkSpace() {

}

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

int RM_FileHandle::init(int _fileId, int _recordSize, int _recordPP, int _recordSum, int _pageCnt, BufType _pageMap, BufPageManager *_bufpm) 
{
	fileId = _fileId;
	recordSize = _recordSize;
	recordPP = _recordPP;
	recordSum = _recordSum;
	pageCnt = _pageCnt;
	pageUintMap = _pageMap;
	pageBitMap = new MyBitMap((PAGE_INT_NUM-3)<<5,pageUintMap);
	mBufpm = _bufpm;
	if(recordPP%32 == 0)
		recordMapSize = recordPP/32;
	else
		recordMapSize = recordPP/32+1;
	recordUintMap = new uint[recordMapSize];
	return 0;
}


int RM_FileHandle::InsertRec(const RM_Record& pData){
	int dataSize;
	cout<<"PP"<<this->recordPP<<endl;
	cout<<"mapSize"<<this->recordMapSize<<endl;
	pData.GetSize(dataSize);
	if(dataSize != this->recordSize)
		return 1;
	RID dataId;
	pData.GetRid(dataId);
	int page,slot;
	if(dataId.GetPageNum(page) > 0 || dataId.GetSlotNum(slot) > 0) {
		return 1;
	}
	if(slot >= this->recordPP)
		return 1;
	BufType bufData = pData.GetData();
	//cout << "buf" << bufData[0] << bufData[1] << endl;
	if (page >= this->pageCnt)
		this->mBufpm->fileManager->writePage(this->fileId, page, reset, 0);
	int bufIndex;
	BufType readBuf = this->mBufpm->getPage(this->fileId,page,bufIndex);
	for (int i = 0; i < this->recordMapSize; i++) {
		recordUintMap[i] = readBuf[i];
	}
	recordBitMap = new MyBitMap(recordMapSize<<5,recordUintMap);
	recordBitMap->setBit(slot, 1);
	//recordBitMap->show();
	//cout << "record"<<recordUintMap[0] << endl;
	for (int i = 0; i < recordSize; i++) {
		readBuf[recordMapSize + i + slot * recordSize] = bufData[i];
		//cout <<"data"<< bufData[i] << endl;
	}
	for (int i = 0; i < this->recordMapSize; i++) {
		readBuf[i] = recordUintMap[i];
	}
	this->mBufpm->markDirty(bufIndex);
	//cout<<page<<" "<<slot<<endl;

/*	FileManager *fm = new FileManager();
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
	fm->writePage(this->fileId, page, buf, 0);*/
	return 0;
}

int RM_FileHandle::DeleteRec(const RID &rid){

	return 0;
}

int RM_FileHandle::UpdateRec(const RM_Record &rec){
	return 0;
}

void RM_FileHandle::show(){
	cout<<fileId<<endl;
	cout<<recordPP<<endl;
	cout<<recordSize<<endl;
}
