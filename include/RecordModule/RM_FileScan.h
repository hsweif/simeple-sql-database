#ifndef RM_FILESCAN
#define RM_FILESCAN

#include "RM_FileHandle.h"
#include "RM_Record.h"
#include "RID.h"

#include <iostream>
#include <vector>

using namespace std;
class RM_FileScan {
private:
    int key;
    int type;
    RID curRecord;
    vector<int> typeArr;
public:
    RM_FileScan();
    RM_FileScan(vector<int> tp);
    int OpenScan(RM_FileHandle &fileHandle, int attrKey, int attrType, int compOp);
    int GetNextRec(RM_Record &rec); // Get next matching record
    int CloseScan(); // Terminate file scan
};
#endif // RM_FILESCAN