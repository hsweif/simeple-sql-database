#ifndef RM_FILEHANDLE_H
#define RM_FILEHANDLE_H
#include "RID.h"
#include "RM_Record.h"
#include "../fileio/FileManager.h"
#include "../bufmanager/BufPageManager.h"
class RM_FileHandle {
private:
  int fileId;
  int recordSize;
  int recordPP;
  int recordSum;
  int pageCnt;
  BufType pageMap;
public:
  RM_FileHandle  (){};                                  // Constructor
  ~RM_FileHandle (){};                                  // Destructor
  int init(int _fileId, int _recordSize, int _recordPP, int _recordSum, int _pageCnt, BufType _pageMap);
  int GetRec         (const RID &rid, RM_Record &rec) const;
  // Get a record
  int InsertRec      (const char *pData, RID &rid);       // Insert a new record,
  //   return record id
  int DeleteRec      (const RID &rid);                    // Delete a record
  int UpdateRec      (const RM_Record &rec);              // Update a record
  //int ForcePages     (PageNum pageNum = ALL_PAGES) const; // Write dirty page(s)
  //   to disk
};
#endif