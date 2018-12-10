#include "RecordModule/RM_FileHandle.h"
#include <iostream>
using namespace std;
BufType reset;
RM_FileHandle::RM_FileHandle() {
	if (reset == NULL) {
		reset = new uint[PAGE_INT_NUM];
	}
	for (int i = 0; i < PAGE_INT_NUM; i++) {
		reset[i] = 0xFFFFFFFF;
	}

}

RM_FileHandle::RM_FileHandle(int id, int sz)
{
	this->fileId = id;
	this->recordSize = sz;
}	
int RM_FileHandle::init(int _fileId, BufPageManager *_bufpm, char *indexName)
{
	this->indexPath = string(indexName);
	fileId = _fileId;
	mBufpm = _bufpm;
	BufType firstPage = mBufpm->getPage(fileId, 0, firstPageBufIndex);
	recordSize = firstPage[0];
	recordPP = firstPage[1];
	recordSum = firstPage[2];
	pageCnt = firstPage[3];
	colNum = firstPage[4];
	cout << recordSize << " " << colNum << endl;
	type.clear();
	title.clear();
	/**
	 * 接下来colNum个uint定义type，在colNum个ITEM_TYPE长度的title
	 */
	for(int i = 0; i < colNum; i ++) {
		type.push_back(firstPage[HEAD_OFFSET+i]);
		BufType colName = &firstPage[HEAD_OFFSET+(i*(ITEM_LENGTH/4))+colNum];
		uint mask = (1<<8) - 1;
		char c[16];
		int cnt = 0;
		for(int k = 0; k < ITEM_LENGTH / 4; k++) {
		    for(int shift = 0; shift < 32; shift += 8) {
		    	uint num = ((colName[k] >> shift) & mask);
				if(isValidChar(num)) {
					c[cnt] = (char)num;
					cnt ++;
				}
				else {
					break;
				}
			}
		}
		string str(c);
		title.push_back(str);
	}
	int offset = HEAD_OFFSET + colNum + (ITEM_LENGTH/4)*colNum;
	pageUintMap = new uint[PAGE_INT_NUM - offset];
	for (int i = 0; i < PAGE_INT_NUM - offset; i++) {
		pageUintMap[i] = firstPage[i + offset];
	}
	pageBitMap = new MyBitMap((PAGE_INT_NUM - offset) << 5,pageUintMap);
	if(recordPP%32 == 0) {
		recordMapSize = recordPP/32;
	}
	else {
		recordMapSize = recordPP/32+1;
	}
	recordUintMap = new uint[recordMapSize];
	cout << "Length: " << colNum << " " << type.size() << endl;
	return 0;
}

bool RM_FileHandle::isValidChar(uint c)
{
	if(c >= 32 && c <= 124)
		return true;
	else
		return false;
}

int RM_FileHandle::updateHead() {
	BufType readBuf = this->mBufpm->getPage(this->fileId, 0, firstPageBufIndex);
	readBuf[0] = recordSize;
	readBuf[1] = recordPP;
	readBuf[2] = recordSum;
	readBuf[3] = pageCnt;
	readBuf[4] = colNum;
	/**
	 * 接下来colNum个uint定义type，在colNum个ITEM_TYPE长度的title
	 */
	for(int i = 0; i < colNum; i ++) {
	    readBuf[HEAD_OFFSET+i] = type[i];
		int cnt = 0, l = title[i].length();
		cout << "test" << title[i] << endl;
		for(int k = 0; k < ITEM_LENGTH / 4; k++) {
		    int pos = HEAD_OFFSET + colNum + (i*(ITEM_LENGTH/4)) + k;
			readBuf[pos] = 0;
		    for(int shift = 0; shift < 32; shift += 8) {
		        if(cnt < l) {
                    readBuf[pos] += ((uint)title[i][cnt] << shift);
                    cnt ++;
				}
			}
		}
	}
	int offset = HEAD_OFFSET + colNum + (ITEM_LENGTH/4)*colNum;
	for (int i = 0; i < PAGE_INT_NUM - offset; i++) {
		readBuf[i + offset] = pageUintMap[i];
	}
	this->mBufpm->markDirty(firstPageBufIndex);
	return 0;
}


int RM_FileHandle::GetRec(const RID &rid, RM_Record &rec)
{
	int page, slot;
	rid.GetPageNum(page);
	rid.GetSlotNum(slot);
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
	return 0;
}

vector<int> RM_FileHandle::GetType()
{
	return this->type;
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



int RM_FileHandle::InsertRec(RM_Record& pData){
	//check size
	int dataSize;
	pData.GetSize(dataSize);
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

	 // TODO: support different key value
	 // this->indexHandle->InsertRecord(pData);
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
	recordSum--;
	pageBitMap->setBit(page - 1, 1);
	this->mBufpm->markDirty(bufIndex);
	bufLastIndex = bufIndex;

	// TODO: remove from index

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

int RM_FileHandle::RecordNum() const
{
	return recordSum;
}

void RM_FileHandle::SetType(vector<int> tp)
{
	type = tp;
	colNum = tp.size();
}

void RM_FileHandle::SetTitle(vector<string> t) {
    title = t;
	colNum = t.size();
    this->indexHandle = new IM::IndexHandle(title, this->indexPath);
    this->indexHandle->CreateIndex((char*)title[0].data(), 0);
}

void RM_FileHandle::PrintTitle()
{
	int sz = title.size();
	for(int i = 0; i < sz; i ++) {
		cout << title[i] << " | ";
	}
	cout << endl;
}

int RM_FileHandle::CreateDir(string dirPath)
{
   	string command = "mkdir -p " + dirPath;
    system(command.c_str());
}
