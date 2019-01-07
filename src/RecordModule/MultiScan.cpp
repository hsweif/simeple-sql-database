//
// Created by AlexFan on 2019/1/7.
//

#include "RecordModule/MultiScan.h"

using namespace RM;

ScanQuery::ScanQuery(int mCol, IM::CompOp cp, int vCol)
{
    queryType = RM::BASIC;  
    mainCol = mCol;
    viceCol = vCol;
}

DualScan::~DualScan() {
    // if(mainScan != nullptr) {
    //     delete mainScan;
    // }
    // if(viceScan != nullptr) {
    //     delete viceScan;
    // }
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

int DualScan::OpenScan(list<ScanQuery> queryList)
{
    if(CheckHandler() {
        return 1;
    }
    RM_FileHandle *mHandler = mainScan->fileHandler, *vHandler = viceScan->fileHandler;
    RM_Record rec;
    scanResult = new list<pair<RID, list<RID>>;
    mainScan->CloseScan();
    viceScan->CloseScan();
    mainScan->OpenScanAll(*mHandler);
    while(mainScan->GetNextRec(*mHandler, rec) != 1)
    {
        RID mainRid;
        rec.GetRid(rid);
        for(auto iter = queryList.begin(); iter != queryList.end(); iter ++)
        {
            int mCol = iter->mainCol, vCol = iter->viceCol;
            IM::CompOp compOp = iter->compOp;
            string colStr;
            bool isNull;
            if(mHandler->recordHandler->GetColumnStr(rec, mCol, colStr, isNull)){
                return 1;
            }
            char *cStr = colStr;
            // FIXME: Bug may occur, be aware
            viceScan->OpenScan(*vHandler, vCol, compOp, cStr);
        }
        
        RM_Record vRecord;
        RID vRid;
        list<RID> ridList;
        while(viceScan->GetNextRec(*vHandler, vRecord) != 1)
        {
            vRecord.GetRid(vRid);
            ridList.push_back(vRid);
        }
        scanResult->push_back(pair(mainRid, ridList)); 
    }
    curResult = scanResult->begin();
    return 0;
}

int DualScan::GetNextPair(pair<RID, list<RID>> &item) 
{
    if(curResult == scanResult->end()) {
        return 1; 
    }
    item = *curResult;
    curResult ++;
}

int DualScan::CloseScan() 
{
    if(scanResult != nullptr) {
        delete scanResult; 
    }
    scanResult = nullptr;
}
