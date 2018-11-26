#ifndef RM_FILEHANDLE_H
#define RM_FILEHANDLE_H
#include "RID.h"
#include "RM_Record.h"
#include "../fileio/FileManager.h"
#include "../bufmanager/BufPageManager.h"
#include "../utils/pagedef.h"
#include <iostream>
#include "../utils/MyBitMap.h"

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
public:
    RM_FileHandle();
    RM_FileHandle(int id, int sz);
    ~RM_FileHandle();                                  // Destructor
	int updateHead();
	RM_FileHandle(BufPageManager* bufpm, int fd, int rcz);
    int GetRec(const RID &rid, RM_Record &rec);
    int init(int _fileId, BufPageManager* _bufpm);
    // Get a record
    int InsertRec(const RM_Record& pData);       // Insert a new record,
    //   return record id
    int DeleteRec(const RID &rid);                    // Delete a record
    int UpdateRec(const RM_Record &rec);
    int RecordNum() const;
    int PageNum() const {return pageCnt;}
    void show();
	int GetSlot(BufType page);
    // int ForcePages     (PageNum pageNum = ALL_PAGES) const; // Write dirty page(s)
};
#endif