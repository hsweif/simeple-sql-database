#ifndef RM_FILEHANDLE_H
#define RM_FILEHANDLE_H
#include "RID.h"
#include "RM_Record.h"
#include "../fileio/FileManager.h"
#include "../bufmanager/BufPageManager.h"
#include "../utils/pagedef.h"
#include "../utils/MyBitMap.h"
#include "IndexModule/IndexManager.h"
#include "IndexModule/IndexHandle.h"
#include "RecordHandler.h"
#include "RM_Manager.h"
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <list>
#include <cstdlib>
#include <fstream>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>


using namespace bpt;
using namespace std;

namespace RM
{
	const int TITLE_LENGTH = 16;
}
class RM_Manager;

class RM_FileHandle {
private:
    bool isInitialized;
    int fileId;
    int recordSize;
    int recordPP;
    int recordSum;
    int pageCnt;
    std::vector<uint> mainKey;
    int mainKeyCnt;
    int recordMapSize;
	int firstPageBufIndex;
	int bufLastIndex = -1;
	int colNum;
	uint *pageUintMap;
	uint *recordUintMap;
	int foreignKeyNum;
    MyBitMap* pageBitMap;
    MyBitMap* recordBitMap;//current reading page's map
    BufPageManager *mBufpm;
	BufType readBuf;
	vector<string> title;
	int CheckForMainKey(RM_Record &pData);
	int CheckForForeignKey(RM_Record &rec, IM::IndexAction action);
	map<string, int> colNameMap;
	string tableName;
	BufType reset;
	vector< pair<int, pair<string, int>> > foreignKey;

public:
	string indexPath;
	RM_Manager *relatedRManager;
	IM::IndexHandle *indexHandle;
    RM::RecordHandler *recordHandler;
	int CheckForMainKey();
    RM_FileHandle(bool _init = true);
    ~RM_FileHandle();                                  // Destructor
	int updateHead();
	// RM_FileHandle(BufPageManager* bufpm, int fd, int rcz);
    int GetRec(const RID &rid, RM_Record &rec);
    int init(int _fileId, BufPageManager* _bufpm, string tableName);
    int InsertRec(RM_Record& pData);       // Insert a new record,
    int InsertRec(vector<string> attrNames, vector<RM_node> nodes);
    int DeleteRec(const RID &rid);                    // Delete a record
    int UpdateRec(RM_Record &rec);
    int RecordNum() const;
    int PrintAttribute(const string &attrName, RM_Record &rec);
    int PrintChartInfo(string chartName);
    int PrintColumnInfo();
    int PageNum() const {return pageCnt;}
    int GetAttrIndex(const string &attrName, int &index);
    bool isMainKey(uint key);
    void SetTitle(vector<string> t);
    int AddForeignKey(RM_Manager *rmg, string chartName, string attrName, int col);
    int GetForeignKeyInfo(int pos, pair<string, int> &info);
    int InitIndex(bool forceEmpty = false);
    void PrintTitle();
    void PrintTitle(vector<int> colName);
    void SetFilePath();
    void SetType(vector<int> tp);
	int SetMainKey(std::vector<int> mainKeys);
    void show();
	int GetSlot(BufType page);
	static int CreateDir(string dirPath);
	int GetAllRecord(vector<RM_Record> &result);
	int GetAllRid(list<RID> *result);
    // int ForcePages     (PageNum pageNum = ALL_PAGES) const; // Write dirty page(s)
};

#endif