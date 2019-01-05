#ifndef RM_FILESCAN
#define RM_FILESCAN

#include "RM_FileHandle.h"
#include "RM_Record.h"
#include "RID.h"
#include "config.h"

#include <iostream>
#include <vector>
#include <list>

using namespace std;
class RM_FileScan {
private:
    list<RID>::iterator curResult;
    list<RID> *scanResult;
    bool noScanBefore;
public:
    RM_FileScan();
    ~RM_FileScan();
    int OpenScan(RM_FileHandle &fileHandle, int col, IM::CompOp comOp, char *value);
    int GetNextRec(RM_FileHandle &fileHandle, RM_Record &rec); // Get next matching record
    int CloseScan(); // Terminate file scan
};
#endif // RM_FILESCAN