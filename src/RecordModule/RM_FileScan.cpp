#include "RecordModule/RM_FileScan.h"

RM_FileScan::RM_FileScan()
{
    this->scanResult = new list<RID>();
    resultNum = 0;
    noScanBefore = true;
    fileHandler = nullptr;
}

RM_FileScan::~RM_FileScan()
{
    delete this->scanResult;
}

int RM_FileScan::SetFilehandler(RM_FileHandle &fileHandle)
{
    if(fileHandler = nullptr) {
        *fileHandler = fileHandle;
        return 0;
    }
}

int RM_FileScan::OpenScan(RM_FileHandle &fileHandle, int col, IM::CompOp comOp, char *value)
{
    SetFilehandler(fileHandle);
    if(noScanBefore && fileHandle.indexHandle->IsIndex(col))
    {
        if(comOp == IM::LS || comOp == IM::LEQ)
        {
            fileHandle.indexHandle->SearchRange(*scanResult, "", value, comOp, col);
            curResult = scanResult->begin();
        }
        else if(comOp == IM::GT || comOp == IM::GEQ)
        {
            char *maxKey = "~~~~~~~~~~~~~~~";
            fileHandle.indexHandle->SearchRange(*scanResult, maxKey, value ,comOp, col);
            curResult = scanResult->begin();
        }
        else if(comOp == IM::EQ)
        {
            fileHandle.indexHandle->SearchRange(*scanResult, value, value, comOp, col);
            curResult = scanResult->begin();
        }
        if(comOp == IM::LS || comOp == IM::GT)
        {
            string vStr(value);
            if(comOp == IM::LS) {
                scanResult->reverse();
                curResult = scanResult->begin();
            }
            while(curResult != scanResult->end())
            {
                RM_Record record;
                if(fileHandle.GetRec(*curResult, record)) {
                    cout << "Error in scanning" << endl;
                    return 1;
                }
                RM_node node;
                fileHandle.recordHandler->GetColumn(col, record, node);
                if(!node.CmpCtx(IM::EQ, vStr)) {
                    break;
                }
                scanResult->erase(curResult);
                curResult ++;
            }
            if(comOp == IM::LS) {
                scanResult->reverse();
                curResult = scanResult->begin();
            }
        }
        noScanBefore = false;
    }
    else
    {
        if(noScanBefore) {
            fileHandle.GetAllRid(scanResult);
        }
        curResult = scanResult->begin();
        while(curResult != scanResult->end())
        {
            RM_Record record;
            if(fileHandle.GetRec(*curResult, record)) {
                cout << "Error in scanning" << endl;
                return 1;
            }
            RM_node node;
            fileHandle.recordHandler->GetColumn(col, record, node);
            if(!node.CmpCtx(comOp, value)) {
                scanResult->erase(curResult);
            }
            curResult ++;
        }
        curResult = scanResult->begin();
    }
    return 0;
}

int RM_FileScan::OpenScan(RM_FileHandle &fileHandle, int col, bool isNull)
{
    SetFilehandler(fileHandle);
    if(noScanBefore || !fileHandle.indexHandle->IsIndex(col)) {
        fileHandle.GetAllRid(scanResult);
    }
    for(auto iter = scanResult->begin(); iter != scanResult->end(); iter++)
    {
        RM_Record record;
        if(fileHandle.GetRec(*iter, record)) {
            cout << "Error in scanning for null" << endl;
            return 1;
        }
        if(record.IsNull(col) && !isNull) {
            scanResult->erase(iter);
        }
        else if(!record.IsNull(col) && isNull) {
            scanResult->erase(iter);
        }
    }
    curResult = scanResult->begin();
    return 0;
}

int RM_FileScan::OpenScanAll(RM_FileHandle &fileHandle)
{
    SetFilehandler(fileHandle);
    if(noScanBefore) {
        fileHandle.GetAllRid(scanResult);
    }
    curResult = scanResult->begin();
}

int RM_FileScan::GetNextRec(RM_FileHandle &fileHandle, RM_Record &rec)
{
    SetFilehandler(fileHandle);
    RM_Record nRec;
    if(curResult != scanResult->end() && !scanResult->empty()) {
        if(fileHandle.GetRec(*curResult, nRec)) {
            return 1;
        }
        curResult ++;
        rec = nRec;
        return 0;
    }
    else{
        return 1;
    }
}

int RM_FileScan::CloseScan()
{
    scanResult->clear();
    noScanBefore = true;
    fileHandler = nullptr;
}
