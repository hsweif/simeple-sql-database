#include "RecordModule/RM_FileHandle.h"
#include <iostream>
#define MAP_SIZE 1000

using namespace std;
RM_FileHandle::RM_FileHandle(bool _init) {

	reset = new uint[PAGE_INT_NUM];
	for (int i = 0; i < PAGE_INT_NUM; i++) {
		reset[i] = 0xFFFFFFFF;
	}
	this->isInitialized = _init;
	this->recordHandler = nullptr;
	this->foreignKeyNum = 0;
	this->relatedRManager = nullptr;
	this->recordBitMap = nullptr;
	this->pageBitMap = nullptr;
}

RM_FileHandle::~RM_FileHandle()
{
	// AWARE
    if(pageUintMap != nullptr) { delete [] pageUintMap; }
    if(recordUintMap != nullptr) { delete [] recordUintMap; }
    if(recordHandler != nullptr) { delete recordHandler; }
    if(pageBitMap != nullptr) {delete pageBitMap;}
    if(recordBitMap != nullptr) {delete recordBitMap;}
	if(relatedRManager != nullptr) {delete relatedRManager;}
	delete reset;
}

int RM_FileHandle::init(int _fileId, BufPageManager *_bufpm, string tableName)
{
	// this->indexPath = string(indexName);
	this->tableName = tableName;
	fileId = _fileId;
	mBufpm = _bufpm;
	BufType firstPage = mBufpm->getPage(fileId, 0, firstPageBufIndex);
	recordSize = firstPage[0];
	recordPP = firstPage[1];
	recordSum = firstPage[2];
	pageCnt = firstPage[3];
	colNum = firstPage[4];

	if(colNum > 0 && (recordHandler == NULL || (recordHandler != NULL && !recordHandler->isInitialized))) {
		recordHandler = new RM::RecordHandler(colNum);
	}
	recordHandler->SetRecordSize(recordSize);

	if(isInitialized) {
		this->indexHandle = new IM::IndexHandle(colNum);
		if(IM::IndexManager::GetIndexHandler(tableName, this->indexHandle)){
			// IM::IndexManager::SetIndexHandler(tableName, this->indexHandle);
		}
	    // this->indexHandle = new IM::IndexHandle(colNum);
    }

	vector<string> tmpTitle;
	/**
	 * 接下来colNum个uint定义type，再colNum个ITEM_TYPE长度的title
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
		tmpTitle.push_back(str);
	}
	if(!recordHandler->isInitialized) {
		this->SetTitle(tmpTitle);
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
	if(isInitialized) {
		mainKeyCnt = (uint)firstPage[offset];
	}
	offset ++;
	if(isInitialized)
	{
        mainKey.clear();
        for(int i = 0; i < mainKeyCnt; i++){
            uint pos = (uint)firstPage[offset];
            mainKey.push_back(pos);
            indexHandle->SetIndex(pos, true);
            offset++;
        }
        InitIndex(false);
	}
	else{
	    offset += mainKeyCnt;
	}

	// Below is for foreign key
	if(isInitialized) {
		foreignKeyNum = (uint)firstPage[offset];
	}
	offset ++;
	if(isInitialized){
		foreignKey.clear();
		for(int i = 0; i < foreignKeyNum; i ++)
		{
			int colIndex = (int)firstPage[offset];
			offset ++;
			int fIndex = (int)firstPage[offset];
			offset ++;
			string fChart = "";
			int l = RM::TITLE_LENGTH % 4 ? RM::TITLE_LENGTH/4 + 1 : RM::TITLE_LENGTH/4;
			bool flag = false;
			for(int i = 0; i < l; i ++)
			{
			    if(flag) {
			    	break;
				}
				uint ctx = firstPage[offset+i];
				int mask = 255;
				for(int shift = 0; shift < 32; shift += 8)
				{
					char c = (char)(((mask << shift) & ctx) >> shift);
					if(c == '\0') {
						flag = true;
						break;
					}
					else{
						fChart += c;
					}
				}
			}
			offset += l;
			pair<string, int> fInfo(fChart, fIndex);
			pair<int, pair<string, int>> item(colIndex, fInfo);
			foreignKey.push_back(item);
			// TODO: Check validility
			this->indexHandle->SetIndex(colIndex, true);
		}
	}
	else{
		int l = RM::TITLE_LENGTH % 4 ? RM::TITLE_LENGTH/4 + 1 : RM::TITLE_LENGTH/4;
		offset += foreignKeyNum * (2 + l);
	}

	// Below is for mutable item length;
	for(int i = 0; i < colNum; i ++) {
	    int l = (int)firstPage[i + offset];
	    if(l != -1) {
			recordHandler->SetItemLength(i, l);
		}
	}

	// Below is for mapping
	offset += colNum;
	// FIXME
	// int mpsz = ((PAGE_INT_NUM - offset) >> 5);
	int mpsz = MAP_SIZE;
	pageUintMap = new uint[mpsz];
	for (int i = 0; i < mpsz; i++) {
		pageUintMap[i] = firstPage[i + offset];
	}
	// pageBitMap = new MyBitMap((PAGE_INT_NUM - offset) << 5,pageUintMap);
    // pageBitMap = new MyBitMap(PAGE_INT_NUM - offset, pageUintMap);
	pageBitMap = new MyBitMap(mpsz << 5, pageUintMap);
	if(recordPP%32 == 0) {
		recordMapSize = recordPP/32;
	}
	else {
		recordMapSize = recordPP/32+1;
	}
	recordUintMap = new uint[recordMapSize];

	// IM::IndexManager::SetIndexHandler(tableName, this->indexHandle);

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
		// cout << "Update title to head: " << title[i] << l << endl;
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
	readBuf[offset] = mainKeyCnt;
	offset ++;
	for(int i = 0;i<mainKeyCnt;i++){
		readBuf[offset] = mainKey[i];	
		offset++;
	}

	// This is for foreign key
	readBuf[offset] = foreignKeyNum;
	offset ++;
    for(int i = 0; i < foreignKeyNum; i ++)
    {
        pair<int, pair<string, int>> item = foreignKey[i];
        readBuf[offset] = item.first;
        offset ++;
		readBuf[offset] = item.second.second;
		int s_length = item.second.first.length();
        offset ++;
        int l = RM::TITLE_LENGTH % 4 ? RM::TITLE_LENGTH/4 + 1 : RM::TITLE_LENGTH/4;
        bool flag = false;
        int cnt = 0;
        for(int i = 0; i < l; i ++)
        {
            readBuf[i + offset] = 0;
            if(!flag)
			{
                int mask = 255;
                for(int shift = 0; shift < 32; shift += 8)
                {
                    char c;
                    if(cnt == s_length) {
                        flag = true;
                        c = '\0';
                        readBuf[i+offset] += (uint)((c & mask) << shift);
                        break;
                    }
                    else{
						c = item.second.first[cnt];
						readBuf[i+offset] += (uint)((c & mask) << shift);
                        cnt ++;
                    }
                }
			}
        }
        offset += l;
    }

	// This is for mutable item length;
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
	if (page <= 0 || page > pageCnt || slot < 0 || slot >= recordPP) {
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
	delete [] pData;
	return 0;
}

int RM_FileHandle::SetMainKey(std::vector<int> mainKeys)
{
	mainKey.clear();
	for(int key: mainKeys){
		if(key < 0 || key > colNum) {
			return 1;
		}
		mainKey.push_back((uint)key);
		indexHandle->SetIndex(key, true);
	}
	mainKeyCnt = (int)mainKey.size();
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
	BufType upBuf = rec.GetBuf();
    // BufType upBuf = rec.GetData();
	for (int i = 0; i < recordSize; i++) {
        readBuf[recordMapSize + i + slot * recordSize] = upBuf[i];
    }
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

int RM_FileHandle::CheckForForeignKey(RM_Record &rec, IM::IndexAction action)
{
	if(foreignKeyNum == 0) {
		return 0;
	}
	if(this->relatedRManager == nullptr) {
		return 1;
	}
	for(auto iter = foreignKey.begin(); iter != foreignKey.end(); iter ++)
	{
	    int colIndex = iter->first;
	    bool isNull;
	    RM_FileHandle *handler = new RM_FileHandle();
	    string chartStr = iter->second.first;
        int l = chartStr.length();
		char *chartName = new char[l];
		strcpy(chartName, chartStr.c_str());
	    if(!relatedRManager->openFile(chartName, *handler)) {
			delete [] chartName;
			relatedRManager->closeFile(*handler);
			delete handler;
			return 1;
		}

		string cStr;
	    recordHandler->GetColumnStr(rec, colIndex, cStr, isNull);
        l = cStr.length();
        char *colKey = new char[l];
        strcpy(colKey, cStr.c_str());
	    if(isNull) {
			delete [] colKey;
			delete [] chartName;
			relatedRManager->closeFile(*handler);
			delete handler;
	    	return 1;
		}
		int pos = iter->second.second;
		bool res = handler->indexHandle->Existed(pos, colKey);
		// bool res = true;
        if(action == IM::IndexAction::INSERT){
            if(!res) {
                printf("Reference key doesn't exist. It is unable to insert.\n");
				delete [] colKey;
				delete [] chartName;
				relatedRManager->closeFile(*handler);
				delete handler;
            	return 1;
			}
        }
        else if(action == IM::IndexAction::DELETE) {
			if(res) {
                printf("Reference key still exist. It is unable to delete.\n");
				delete [] colKey;
				delete [] chartName;
				relatedRManager->closeFile(*handler);
				delete handler;
				return 1;
			}
        }
        else if(action == IM::IndexAction::UPDATE) {
        	if(!res) {
                printf("Reference key doesn't exist. It is unable to update.\n");
				delete [] colKey;
				delete [] chartName;
				relatedRManager->closeFile(*handler);
				delete handler;
        		return 1;
			}
        }
        delete [] colKey;
        delete [] chartName;
		relatedRManager->closeFile(*handler);
		delete handler;
	}
	return 0;
}

int RM_FileHandle::CheckForMainKey(RM_Record &pData)
{
	if(mainKey.size() != 0)
	{
		bool exist = true;
		for(uint key:mainKey){
	        RM_node mkTest;
	        recordHandler->GetColumn(key, pData, mkTest);
	        string keyStr = "";
	        std::stringstream ss;
	        if(mkTest.type == RM::INT) {
	            ss << mkTest.num;
	            ss >> keyStr;
	        }
	        else if(mkTest.type == RM::FLOAT) {
	            ss << mkTest.fNum;
	            ss >> keyStr;
	        }
	        else{
	            keyStr = mkTest.str;
	        }		
	        char *keyChar = new char[keyStr.length()];
	        for(int i = 0; i < keyStr.length(); i ++) {
	            keyChar[i] = keyStr[i];
	        }	
	        if(key < this->colNum && key >= 0 && !indexHandle->Existed(key, keyChar)){
	            exist = false;
	            break;
	        }
		}
		if(exist)
			return 1;
        else{
        	return 0;
        }
	} else{
		return 0;
	}
}

int RM_FileHandle::InsertRec(RM_Record& pData){

	//check size
	int dataSize = pData.BufSize();
	if(dataSize != this->recordSize)
	{
		printf("data size error: %d''\n", dataSize, this->recordSize);
		return 1;
	}

	if(CheckForMainKey(pData)) {
		printf("Item with same main key already existed\n");
		return 1;
	}

	if(CheckForForeignKey(pData, IM::IndexAction::INSERT)) {
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
	BufType bufData = pData.GetBuf();
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
	if(CheckForForeignKey(record, IM::IndexAction::DELETE)) {
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

bool RM_FileHandle::isMainKey(uint key){
	for(uint mkey: mainKey){
		if(key == mkey){
			return true;
		}
	}
	return false;
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
	for(int i = 0; i < title.size(); i ++) {
		colNameMap.insert(pair<string, int>(title[i], i));
	}
}

int RM_FileHandle::InitIndex(bool forceEmpty)
{
	if(title.empty() || colNum <= 0) {
		return 1;
	}
	this->indexHandle->SetIndexHandle(title, this->indexPath);

	if(IM::IndexManager::GetIndexHandler(tableName, this->indexHandle)) {
		for (int i = 0; i < title.size(); i++) {
			this->indexHandle->CreateIndex((char *) title[i].data(), i, forceEmpty);
		}
		IM::IndexManager::SetIndexHandler(this->tableName, this->indexHandle);
	}
	return 0;
}

void RM_FileHandle::PrintTitle()
{
	int sz = title.size();
	RM::ItemType *type = recordHandler->GetItemType();
	int *length = recordHandler->GetItemLength();
	string splitLine = "+";
	for(int i = 0; i < sz; i ++) {
	    splitLine += recordHandler->GetSplitLine(i);
    }
    cout << splitLine << endl;
	cout << "|";
	for(int i = 0; i < sz; i ++) {
	    int width = (type[i] == RM::CHAR && length[i] > ALIGN_WIDTH) ? length[i] : ALIGN_WIDTH;
		cout << setiosflags(ios::left) << setw(width) << title[i];
		cout << "|";
	}
	cout << endl;
	cout << splitLine << endl;
}

void RM_FileHandle::PrintTitle(vector<int> colIndex)
{
    string splitLine = "+";
    for(auto iter = colIndex.begin(); iter != colIndex.end(); iter ++) {
        splitLine += recordHandler->GetSplitLine(*iter);
    }
    cout << splitLine << endl;
    cout << "|";
    RM::ItemType *type = recordHandler->GetItemType();
    int *length = recordHandler->GetItemLength();
    for(auto iter = colIndex.begin(); iter != colIndex.end(); iter ++) {
        int i = *iter;
        int width = (type[i] == RM::CHAR && length[i] > ALIGN_WIDTH) ? length[i] : ALIGN_WIDTH;
        cout << setiosflags(ios::left) << setw(width) << title[i];
        cout << "|";
    }
    cout << endl;
    cout << splitLine << endl;
}

int RM_FileHandle::GetAllRecord(vector<RM_Record> &result)
{
	int sum = this->RecordNum();
    int pageCount = this->PageNum();
    int recordPP = this->recordPP;
    int page = 1, slot = 0;
    int cnt = 0;

    result.clear();

    while(cnt < sum)
    {
    	if(slot >= recordPP) {
    		page ++;
    		slot = 0;
		}
        RID rid(page, slot);
        RM_Record record;
        if(this->GetRec(rid, record) == 0)
        {
            result.push_back(record);
            slot ++;
            cnt ++;
        }
    }

    return 0;
}

int RM_FileHandle::GetAllRid(list<RID> *result)
{
	int sum = this->RecordNum();
    int pageCount = this->PageNum();
    int recordPP = this->recordPP;
    int page = 1, slot = 0;
    int cnt = 0;

    result->clear();

    while(cnt < sum)
    {
    	if(slot >= recordPP) {
    		page ++;
    		slot = 0;
		}
        RID rid(page, slot);
        RM_Record record;
        if(this->GetRec(rid, record) == 0)
        {
            result->push_back(rid);
            cnt ++;
        }
        slot ++;
    }

    return 0;
}
int RM_FileHandle::CreateDir(string dirPath)
{
#ifdef __DARWIN_UNIX03
    mkdir(dirPath.c_str(), S_IRWXU);
#endif
#ifdef WIN32
	if(_mkdir(dirPath.c_str()) != 0){
		cout<<"createDir error"<<endl;
		return -1;
	}
#endif
}

int RM_FileHandle::PrintColumnInfo()
{
	if(recordHandler->itemNum != colNum) {
		return 1;
	}
	RM::ItemType *itemType = recordHandler->GetItemType();
	int *itemLength = recordHandler->GetItemLength();
	string colInfo = "| ";
	string splitLine = "+-";
	for(int i = 0; i < colNum; i ++)
    {
	    int info_l = colInfo.length();
        colInfo += title[i];
        if(isMainKey(i)) {
        	colInfo += ", PRIMARY KEY";
		}

        if(recordHandler->IsAllowNull(i)) {
        	colInfo += ", ALLOW NULL";
		}
		else{
			colInfo += ", NOT ALLOW NULL";
		}
		pair<string, int> foreignInfo;
        if(GetForeignKeyInfo(i, foreignInfo) == 1)
        {
            stringstream ss;
            string foreColStr;
            ss << foreignInfo.second;
            ss >> foreColStr;
            colInfo = colInfo + ", REFERENCE KEY " + foreignInfo.first + " COLUMN " + foreColStr;
        }
        if(itemType[i] == RM::INT) {
        	colInfo += ", INT";
		}
		else if(itemType[i] == RM::FLOAT) {
			colInfo += ", FLOAT";
		}
		else if(itemType[i] == RM::CHAR) {
		    string l_str;
		    std::stringstream ss;
		    ss << itemLength[i];
		    ss >> l_str;
		    colInfo = colInfo + ", CHAR(" + l_str + ")";
		}
		colInfo += "|";
		info_l = colInfo.length() - info_l;
		for(int k = 0; k < info_l; k ++) {
		    if(k != info_l - 1) {
                splitLine += "-";
            }
            else{
                splitLine += "+";
            }
        }
    }
    cout << splitLine << endl;
    cout << colInfo << endl;
    cout << splitLine << endl;
    return 0;
}

int RM_FileHandle::PrintAttribute(const string &attrName, RM_Record &rec)
{
	int index;
    if(GetAttrIndex(attrName, index)){
    	return 1;
    }
    return recordHandler->PrintColumn(rec, index);
}

int RM_FileHandle::GetAttrIndex(const string &attrName, int &index)
{
	auto iter = colNameMap.find(attrName);
    if(iter == colNameMap.end()) {
    	return 1;
	}
	index = colNameMap[attrName];
	return 0;	
}

int RM_FileHandle::AddForeignKey(RM_Manager *rmg, string chartName, string attrName, int col)
{
	foreignKeyNum ++;
	char *cName = new char[chartName.length()];
	RM_FileHandle *handler = new RM_FileHandle();
	for(int i = 0; i < chartName.length(); i ++) {
		cName[i] = chartName[i];
	}

	if(!rmg->openFile(cName, *handler)) {
		return 1;
	}
	int index;
	if(handler->GetAttrIndex(attrName, index)){
		return 1;
	}
	if(!handler->isMainKey(index)) {
		return 1;
	}
	pair<string, int> relatedCol(chartName, index);
    pair<int, pair<string, int> > newKey(col, relatedCol);
	foreignKey.push_back(newKey);
	indexHandle->SetIndex(col, true);
	rmg->closeFile(*handler);
	delete handler;
	delete[] cName;
   	return 0;
}

int RM_FileHandle::GetForeignKeyInfo(int pos, pair<string, int> &info)
{
	if(pos < 0 || pos >= colNum)	{
		return -1;
	}
	for(auto iter = foreignKey.begin(); iter != foreignKey.end(); iter ++)
	{
		if(iter->first == pos)
		{
			info = iter->second;
			return 1;
		}
	}
	return 0;
}

int RM_FileHandle::PrintChartInfo(string chartName)
{
    int width = 25;
    stringstream ss;
    string splitLine = "+";
    for(int i = 0; i <  2*width; i ++) {
        if(i != width-1) {
            splitLine += "-";
        }
        else{
            splitLine += "+";
        }
    }
    splitLine += "+";
    cout << splitLine << endl;
    cout << setw(width) << left << "|Name " << "|" << right << setw(width) << chartName << "|" << endl;
    cout << splitLine << endl;
    cout << setw(width) << left << "|Record Number " << "|" << right << setw(width) << recordSum << "|" << endl;
    cout << splitLine << endl;
    cout << setw(width) << left << "|Page Number " << "|" << right << setw(width) << pageCnt << "|" << endl;
    cout << splitLine << endl;
    cout << setw(width) << left << "|Record Size(Byte) " << "|" << right << setw(width) << 4*recordSize << "|" << endl;
    cout << splitLine << endl;
    return 0;
}


