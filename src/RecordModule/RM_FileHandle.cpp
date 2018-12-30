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
	this->isInitialized = false;
}

RM_FileHandle::~RM_FileHandle()
{
	// AWARE
	if(isInitialized) {
        delete pageUintMap;
        delete recordUintMap;
        delete recordBitMap;
        delete mBufpm;
        delete recordHandler;
	}
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

	if(colNum > 0 && recordHandler != NULL && !recordHandler->isInitialized) {
		recordHandler = new RM::RecordHandler(colNum);
	}

	title.clear();
	/**
	 * 接下来colNum个uint定义type，在colNum个ITEM_TYPE长度的title
	 */
	for(int i = 0; i < colNum; i ++)
	{
		int tp = firstPage[HEAD_OFFSET+i];
		if(tp != -1) {
			recordHandler->SetType(i, (RM::ItemType)firstPage[HEAD_OFFSET+i]);
		}
		BufType colName = &firstPage[HEAD_OFFSET+(i*(RM::TITLE_LENGTH/4))+colNum];
		uint mask = (1<<8) - 1;
		char c[16];
		int cnt = 0;
		memset(c, 0, sizeof(c));
		for(int k = 0; k < RM::TITLE_LENGTH / 4; k++) {
		    for(int shift = 0; shift < 32; shift += 8) {
		    	uint num = ((colName[k] >> shift) & mask);
				if(recordHandler->isValidChar(num)) {
					c[cnt] = (char)num;
					cnt ++;
				}
				else {
					break;
				}
			}
		}
		string str(c);
		// cout << str << endl;
		title.push_back(str);
	}
	// TODO: Add support for null key
	int nullSectLength = ((colNum % 32) == 0) ? colNum/32 : colNum/32 + 1;
	int offset = HEAD_OFFSET + colNum + (RM::TITLE_LENGTH/4)*colNum;
	bool *allowNull = new bool[colNum];
	for(int i = 0, cnt = 0; i < nullSectLength && cnt < colNum; i ++)
	{
		uint curNum = (uint)firstPage[i+offset];
		for(int shift = 0; shift < 32 && cnt < colNum; shift ++)
		{
			allowNull[cnt] = (bool)((curNum & (1 << shift)) >> shift);
			cnt ++;
		}
	}
	recordHandler->SetNullInfo(allowNull, colNum);

	// Below is for main key:
	offset += nullSectLength;
	mainKey = (uint)firstPage[offset];


	// Below is for mutable item length;
	offset ++;
	for(int i = 0; i < colNum; i ++) {
	    int l = (int)firstPage[i + offset];
	    if(l != -1) {
			recordHandler->SetItemLength(i, l);
		}
	}

	// Below is for mapping
	offset += colNum;
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

	isInitialized = true;
	return 0;
}


int RM_FileHandle::updateHead() {
	BufType readBuf = this->mBufpm->getPage(this->fileId, 0, firstPageBufIndex);
	readBuf[0] = (uint)recordSize;
	readBuf[1] = (uint)recordPP;
	readBuf[2] = (uint)recordSum;
	readBuf[3] = (uint)pageCnt;
	readBuf[4] = (uint)colNum;
	/**
	 * 接下来colNum个uint定义type，在colNum个ITEM_TYPE长度的title
	 */
	RM::ItemType *type = recordHandler->GetItemType();

	for(int i = 0; i < colNum; i ++) {
	    readBuf[HEAD_OFFSET+i] = type[i];
		int cnt = 0, l = title[i].length();
		cout << "test " << title[i] << l << endl;
		for(int k = 0; k < RM::TITLE_LENGTH / 4; k++) {
		    int pos = HEAD_OFFSET + colNum + (i*(RM::TITLE_LENGTH/4)) + k;
			readBuf[pos] = 0;
		    for(int shift = 0; shift < 32; shift += 8) {
		        if(cnt < l) {
                    readBuf[pos] += ((uint)title[i][cnt] << shift);
                    cnt ++;
				}
			}
		}
	}
	int offset = HEAD_OFFSET + colNum + (RM::TITLE_LENGTH/4)*colNum;
	// Null section
	int nullSectLength = ((colNum % 32) == 0) ? colNum/32 : colNum/32 + 1;
	for(int i = 0, cnt = 0; i < nullSectLength && cnt < colNum; i ++)
	{
		uint curNum = 0;
		for(int shift = 0; shift < 32 && cnt < colNum; shift ++)
		{
			curNum += (recordHandler->IsAllowNull(cnt) << shift);
			cnt ++;
		}
		readBuf[i + offset] = curNum;
	}

	// This is for main key.
	offset += nullSectLength;
	readBuf[offset] = mainKey;

	// This is for mutable item length;
	offset ++;
	int *itemLength = recordHandler->GetItemLength();
	for(int i = 0; i < colNum; i ++) {
		readBuf[i+offset] = itemLength[i];
	}

	// This is for page mapping
	offset += colNum;
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
	rec.SetRecord(pData, recordSize, colNum);
	RID nRid = rid;
	rec.SetRID(nRid);
	return 0;
}

int RM_FileHandle::SetMainKey(int key)
{
	if(key < 0 || key > colNum) {
		return 1;
	}
	mainKey = (uint)key;
	return 0;
}


int RM_FileHandle::UpdateRec(RM_Record &rec) {
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
	// TODO: Update the index
	this->indexHandle->IndexAction(IM::UPDATE, rec, recordHandler);
	return 0;
}



int RM_FileHandle::InsertRec(RM_Record& pData){
	//check size
	// int dataSize = pData.RecordSize();
	int dataSize = pData.BufSize();
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

	//AWARE
	RID rid(page, slot);
	pData.SetRID(rid);

	recordBitMap->setBit(slot, 0);
	for (int i = 0; i < recordSize; i++) {
		readBuf[recordMapSize + i + slot * recordSize] = bufData[i];
	}
	for (int i = 0; i < this->recordMapSize; i++) {
		readBuf[i] = recordUintMap[i];
	}
	recordSum++;
	if (recordBitMap->findLeftOne() >= recordPP) {
		pageBitMap->setBit(page - 1, 0);
	}
	this->mBufpm->markDirty(bufIndex);
	bufLastIndex = bufIndex;

	// TODO: support different key value
	this->indexHandle->IndexAction(IM::INSERT, pData, recordHandler);
	return 0;
}

int RM_FileHandle::DeleteRec(const RID &rid) {
	int page, slot;
	RM_Record record;
	GetRec(rid, record);
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
	this->indexHandle->IndexAction(IM::DELETE, record, recordHandler);
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
	colNum = tp.size();
	for(int i = 0; i < colNum; i ++) {
		recordHandler->SetType(i, (RM::ItemType)tp[i]);
	}
}



void RM_FileHandle::SetTitle(vector<string> t) {
    title = t;
	colNum = t.size();
    this->indexHandle = new IM::IndexHandle(title, this->indexPath);
    for(int i = 0; i < t.size(); i ++) {
		this->indexHandle->CreateIndex((char*)title[i].data(), i);
	}
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
#ifdef __DARWIN_UNIX03
    mkdir(dirPath.c_str(), S_IRWXU);
#endif
}


