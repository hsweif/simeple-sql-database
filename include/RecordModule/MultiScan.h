//
// Created by AlexFan on 2019/1/7.
//

#ifndef MULTISCAN_H
#define MULTISCAN_H

#include "RM_FileScan.h"
#include "RM_FileHandle.h"
#include "../config.h"

using namespace std;
namespace RM{

enum QueryType{
    SINGLE, DUAL
};

enum ScanTarget{
    BOTH, MAIN, VICE
};

enum ScanType{
    RangeScan, NullScan
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

class SingleScanQuery: ScanQuery
{
public:
    RM::ScanType scanType;
    RM::ScanTarget target;
    int colIndex;
    IM::CompOp compOp;
    char *keyValue;
    bool isNull;
    SingleScanQuery(RM::ScanTarget scanTarget, int col, IM::CompOp cmpOp, char *value);
    SingleScanQuery(RM::ScanTarget scanTarget, int col, bool isNull);
};

class DualScan {
private:
    RM_FileHandle *mainHandler;
    RM_FileHandle *viceHandler;
    list<pair<RID, list<RID> > > *scanResult;
    list<pair<RID, list<RID>>>::iterator curResult;
    int CheckHandler();
public:
    DualScan(RM_FileHandle *mHandle, RM_FileHandle *vHandle);
    ~DualScan();
    int OpenScan(list<ScanQuery> queryList);
    int CloseScan();
    int GetNextPair(pair<RID, list<RID>> &item);
};

};

#endif MULTISCAN_H
