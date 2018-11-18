#include "RecordModule/RM_FileHandle.h"
#include <iostream>
using namespace std;
BufType reset;
RM_FileHandle::RM_FileHandle() {
	if (reset == NULL)
		reset = new uint[PAGE_INT_NUM];
	for (int i = 0; i < PAGE_INT_NUM; i++)
		reset[i] = 0xFFFFFFFF;
}

RM_FileHandle::RM_FileHandle(int id, int sz)
{
	this->fileId = id;
	this->recordSize = sz;
}	

int RM_FileHandle::updateHead() {
	BufType readBuf = this->mBufpm->getPage(this->fileId, 0, firstPageBufIndex);
	readBuf[0] = recordSize;
	readBuf[1] = recordPP;
	cout << "PP" << recordPP << endl;
	readBuf[2] = recordSum;
	cout << "sum" << recordSum << endl;
	readBuf[3] = pageCnt;
	cout << "Cnt" << pageCnt << endl;
	for (int i = 0; i < PAGE_INT_NUM - 4; i++) {
		readBuf[i + 4] = pageUintMap[i];
	}
	this->mBufpm->markDirty(firstPageBufIndex);
}

int RM_FileHandle::GetRec(const RID &rid, RM_Record &rec)
{
	int page, slot;
	rid.GetPageNum(page);
	rid.GetSlotNum(slot);
	printf("left index %d\n", pageBitMap->findLeftOne());
	printf("page %d / slot %d /pageCnt %d/ recordPP %d\n", page,slot, pageCnt, recordPP);
	if (page <= 0 || page >= pageCnt || slot < 0 || slot >= recordPP) {
		cout << "fail to get rec" << endl;
		return 1;
	}
	int bufIndex;
	readBuf = this->mBufpm->getPage(this->fileId, page, bufIndex);
	BufType pData = new uint[recordSize];
	for (int i = 0; i < recordSize; i++) {
		pData[i] = readBuf[recordMapSize + i + slot * recordSize];
	}
	rec.SetRecord(pData, recordSize, rid);
	cout << "get rec: " << rec.GetData() << endl;
	return 0;
}

int RM_FileHandle::UpdateRec(const RM_Record &rec) {
	RID rid;
	rec.GetRid(rid);
	int page, slot;
	rid.GetPageNum(page);
	rid.GetSlotNum(slot);
	if (page <= 0 || page >= pageCnt || slot < 0 || slot >= recordPP) {
		return 1;
	}
	int bufIndex;
	readBuf = this->mBufpm->getPage(this->fileId, page, bufIndex);
	if (bufLastIndex != bufIndex)
	{
		for (int i = 0; i < this->recordMapSize; i++) {
			recordUintMap[i] = readBuf[i];
		}
		recordBitMap = new MyBitMap(recordMapSize << 5, recordUintMap);
	}
	BufType upBuf = rec.GetData();
	for (int i = 0; i < recordSize; i++)
		readBuf[recordMapSize + i + slot * recordSize] = upBuf[i];
	if (recordBitMap->getBit(slot))//1
	{
		recordBitMap->setBit(slot, 0);
		recordSum++;
		if (recordBitMap->findLeftOne() >= recordPP)
			pageBitMap->setBit(page - 1, 0);
	}
	this->mBufpm->markDirty(bufIndex);
	bufLastIndex = bufIndex;
	return 0;
}

int RM_FileHandle::init(int _fileId, BufPageManager *_bufpm) 
{
	fileId = _fileId;
	mBufpm = _bufpm;
	//cout << "test" << endl;
	BufType firstPage = mBufpm->getPage(fileId, 0, firstPageBufIndex);
	recordSize = firstPage[0];
	recordPP = firstPage[1];
	recordSum = firstPage[2];
	pageCnt = firstPage[3];
	pageUintMap = new uint[PAGE_INT_NUM - 4];
	for (int i = 0; i < PAGE_INT_NUM - 4; i++) {
		pageUintMap[i] = firstPage[i + 4];
	}
	pageBitMap = new MyBitMap((PAGE_INT_NUM-4)<<5,pageUintMap);
	if(recordPP%32 == 0)
		recordMapSize = recordPP/32;
	else
		recordMapSize = recordPP/32+1;
	recordUintMap = new uint[recordMapSize];
	return 0;
}

int RM_FileHandle::InsertRec(const RM_Record& pData){
	//check size
	int dataSize;
	// printf("recordSum %d\n", recordSum);
	pData.GetSize(dataSize);
	//FIXME: The datasize and recordSize doesn't equal.
	if(dataSize != this->recordSize)
	{
		printf("data size error: %d/ %d\n", dataSize, this->recordSize);
		return 1;
	}
	//check space
	int pageIndex = pageBitMap->findLeftOne();
	if (pageIndex + 1 >= pageCnt)//current pages can't be used
	{
		this->mBufpm->fileManager->writePage(this->fileId, pageIndex + 1, reset, 0);
		pageCnt = pageIndex + 2;
	}
	int page = pageIndex+1;
	BufType bufData = pData.GetData();
	int bufIndex;
	readBuf = this->mBufpm->getPage(this->fileId, page, bufIndex);
	if (bufLastIndex != bufIndex)
	{
		for (int i = 0; i < this->recordMapSize; i++) {
			recordUintMap[i] = readBuf[i];
		}
		recordBitMap = new MyBitMap(recordMapSize << 5, recordUintMap);
	}
	int slot = recordBitMap->findLeftOne();
	if (slot >= recordPP || slot < 0) {
		cout << "codeError" << endl;
		return 1;
	}
	recordBitMap->setBit(slot, 0);
	for (int i = 0; i < recordSize; i++) {
		readBuf[recordMapSize + i + slot * recordSize] = bufData[i];
	}
	for (int i = 0; i < this->recordMapSize; i++) {
		readBuf[i] = recordUintMap[i];
	}
	recordSum++;
	if (recordBitMap->findLeftOne() >= recordPP)
		pageBitMap->setBit(page - 1, 0);
	this->mBufpm->markDirty(bufIndex);
	bufLastIndex = bufIndex;
	return 0;
}

int RM_FileHandle::DeleteRec(const RID &rid) {
	int page, slot;
	rid.GetPageNum(page);
	rid.GetSlotNum(slot);
	if (page <= 0 || page >= pageCnt || slot < 0 || slot >= recordPP) {
		return 1;
	}
	int bufIndex;
	readBuf = this->mBufpm->getPage(this->fileId, page, bufIndex);
	if (bufLastIndex != bufIndex)
	{
		for (int i = 0; i < this->recordMapSize; i++) {
			recordUintMap[i] = readBuf[i];
		}
		recordBitMap = new MyBitMap(recordMapSize << 5, recordUintMap);
	}
	recordBitMap->setBit(slot, 1);
	//recordBitMap->show();
	recordSum--;
	pageBitMap->setBit(page - 1, 1);
	this->mBufpm->markDirty(bufIndex);
	bufLastIndex = bufIndex;
	return 0;
}

void RM_FileHandle::show(){
	cout<<fileId<<endl;
	cout<<recordPP<<endl;
	cout<<recordSize<<endl;
}

/**
 * TODO: 改写成二级跳转
 * 解析传入的页，回传最左边的空位offset
 * @page 传入一页 
 */
int RM_FileHandle::GetSlot(BufType page)
{
	int offset = 0;
	bool found = false;
	while(!found && offset < PAGE_INT_NUM)
	{
		int upper = offset + recordSize;
		bool flag = true;
		for(int i = offset; i < upper; i ++)
		{
			if(page[i] != 0)
			{
				flag = false;
				break;
			}
		}
		if(!flag)
		{
			offset = upper;
		}
		else
		{
			found = true;
			break;
		}
	}
}
