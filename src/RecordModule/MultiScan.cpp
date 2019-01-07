//
// Created by AlexFan on 2019/1/7.
//

#include "RecordModule/MultiScan.h"

using namespace RM;

DualScan::~DualScan() {
    if(mainScan != nullptr) {
        delete mainScan;
    }
    if(viceScan != nullptr) {
        delete viceScan;
    }
}

DualScan::DualScan(RM_FileScan *mScan, RM_FileScan *vScan) {
    mainScan = mScan;
    viceScan = vScan;
    scanResult = new list<pair<RID, list<RID>>>();
}

int DualScan::CheckHandler()
{
    if(mainScan->fileHandler == nullptr || viceScan->fileHandler == nullptr)   {
        cout << "Fail to open dual scan" << endl;
        return 1;
    }
    return 0;
}

int DualScan::OpenScan(IM::CompOp compOp, int mCol, int vCol)
{
    if(CheckHandler() {
        return 1;
    }

    RM_FileHandle *mHandler = mainScan->fileHandler, *vHandler = viceScan->fileHandler;
    RM_Record rec;
    if(mainScan->noScanBefore && viceScan->noScanBefore)
    {
        mainScan->OpenScanAll(*mHandler);
        while(mainScan->GetNextRec(*mHandler, rec) != 1)
        {
            string mkey;
            if(mHandler->recordHandler->GetColumn())
            viceScan->OpenScan(*vHandler, vCol, compOp)
        }
    }
    return 0;
}
