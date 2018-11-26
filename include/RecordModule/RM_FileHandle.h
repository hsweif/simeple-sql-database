#ifndef RM_FILEHANDLE_H
#define RM_FILEHANDLE_H
#include "RID.h"
#include "RM_Record.h"
#include "../IndexModule/bpt.h"
#include "../fileio/FileManager.h"
#include "../bufmanager/BufPageManager.h"
#include "../utils/pagedef.h"
#include "../utils/MyBitMap.h"
#include <iostream>
#include <string>
#include <vector>

using namespace bpt;
using namespace std;
class RM_FileHandle {
private:
    int fileId;
    int recordSize;
    int recordPP;
    int recordSum;
    int pageCnt;
    int recordMapSize;
	int firstPageBufIndex;
	int bufLastIndex = -1;
	uint* pageUintMap;
	uint* recordUintMap;
    MyBitMap* pageBitMap;
    MyBitMap* recordBitMap;//current reading page's map
    BufPageManager *mBufpm;
	BufType readBuf;
	bplus_tree *indexBPTree;
	vector<string> title;

public:
    RM_FileHandle();
    RM_FileHandle(int id, int sz);
    ~RM_FileHandle();                                  // Destructor
	int updateHead();
	RM_FileHandle(BufPageManager* bufpm, int fd, int rcz);
    int GetRec(const RID &rid, RM_Record &rec);
    int init(int _fileId, BufPageManager* _bufpm, char *indexName);
    // Get a record
    int InsertRec(RM_Record& pData);       // Insert a new record,
    //   return record id
    int DeleteRec(const RID &rid);                    // Delete a record
    int UpdateRec(const RM_Record &rec);
    int RecordNum() const;
    int PageNum() const {return pageCnt;}
    void SetTitle(vector<string> t) {
    	title = t;
	}
    void show();
	int GetSlot(BufType page);
    // int ForcePages     (PageNum pageNum = ALL_PAGES) const; // Write dirty page(s)
};
#endif