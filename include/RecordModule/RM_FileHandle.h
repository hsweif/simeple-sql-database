#ifndef RM_FILEHANDLE_H
#define RM_FILEHANDLE_H
#include "RID.h"
#include "RM_Record.h"
#include "../fileio/FileManager.h"
#include "../bufmanager/BufPageManager.h"
#include "../utils/pagedef.h"

class RM_FileHandle {
private:
    int fileId;
    int recordSize;
    int recordPP;
    int recordSum;
    int pageCnt;
    BufType pageMap;
  
public:
    RM_FileHandle();
    RM_FileHandle(int id, int sz);
    ~RM_FileHandle();                                  // Destructor
    RM_FileHandle(BufPageManager* bufpm, int fd, int rcz);
    int GetRec(const RID &rid, RM_Record &rec) const;
    int init(int _fileId, int _recordSize, int _recordPP, int _recordSum, int _pageCnt, BufType _pageMap);
    // Get a record
    int InsertRec(const RM_Record& pData, RID &rid);       // Insert a new record,
    //   return record id
    int DeleteRec(const RID &rid);                    // Delete a record
    int UpdateRec(const RM_Record &rec);              // Update a record
    // int ForcePages     (PageNum pageNum = ALL_PAGES) const; // Write dirty page(s)
};
#endif