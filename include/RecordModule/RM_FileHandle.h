#ifndef RM_FILEHANDLE_H
#define RM_FILEHANDLE_H
#include "RID.h"
#include "RM_Record.h"
#include "../include/fileio/FileManager.h"
#include "../include/fileio/BufPageManeger.h"
class RM_FileHandle {
private:
  int fileId;
  int recordSize;
  
public:
  RM_FileHandle  ();                                  // Constructor
  ~RM_FileHandle ();                                  // Destructor
  RM_FileHandle(BufPageManager* bufpm,);
  int GetRec         (const RID &rid, RM_Record &rec) const;
  // Get a record
  int InsertRec      (const char *pData, RID &rid);       // Insert a new record,
  //   return record id
  int DeleteRec      (const RID &rid);                    // Delete a record
  int UpdateRec      (const RM_Record &rec);              // Update a record
  int ForcePages     (PageNum pageNum = ALL_PAGES) const; // Write dirty page(s)
  //   to disk
};
#endif