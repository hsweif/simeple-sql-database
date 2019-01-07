//
// Created by AlexFan on 2019/1/7.
//

#ifndef MULTISCAN_H
#define MULTISCAN_H

#include "RM_FileScan.h"
#include "../config.h"

using namespace std;
namespace RM{

enum QueryType{
    BASIC
};

class ScanQuery
{
public:
    QueryType queryType;
    int mainCol;
    IM::CompOp compOp;
    int viceCol;
    ScanQuery(int mCol, IM::CompOp cp, int vCol);
};

class DualScan {
private:
    RM_FileScan *mainScan;
    RM_FileScan *viceScan;
    list<pair<RID, list<RID> > > *scanResult;
    list<pair<RID, list<RID>>>::iterator curResult;
    int CheckHandler();
public:
    DualScan(RM_FileScan *mScan, RM_FileScan *vScan);
    ~DualScan();
    int OpenScan(list<ScanQuery> queryList);
    int CloseScan();
    int GetNextPair(pair<RID, list<RID>> &item);
};

};

#endif MULTISCAN_H
