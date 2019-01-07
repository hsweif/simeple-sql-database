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

namespace RM{
    class DualScan;
};

class RM_FileScan {
private:
    list<RID>::iterator curResult;
    list<RID> *scanResult;
    bool noScanBefore;
    RM_FileHandle *fileHandler;
    int SetFilehandler(RM_FileHandle &fileHandle);
public:
    friend class RM::DualScan;
    RM_FileScan();
    ~RM_FileScan();
    int OpenScan(RM_FileHandle &fileHandle, int col, IM::CompOp comOp, char *value);
    int OpenScan(RM_FileHandle &fileHandle, int col, bool isNull);
    int OpenScanAll(RM_FileHandle &fileHandle);
    int GetNextRec(RM_FileHandle &fileHandle, RM_Record &rec); // Get next matching record
    int CloseScan(); // Terminate file scan
};

#include "MultiScan.h"

#endif // RM_FILESCAN