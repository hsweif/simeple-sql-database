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
    RM::ScanTarget target;
    int mainCol;
    IM::CompOp compOp;
    int viceCol;
    RM::ScanType scanType;
    char *keyValue;
    bool isNull;
    ScanQuery(int mCol, IM::CompOp cp, int vCol);
    ScanQuery(RM::ScanTarget scanTarget, int col, IM::CompOp cmpOp, char *value)
    {
        queryType = SINGLE;
        scanType = RangeScan;
        keyValue = new char[strlen(value)];
        strcpy(keyValue, value);
        compOp = cmpOp;
        target = scanTarget;
        if(target == MAIN) {
            mainCol = col;
        }
        else if(target == VICE) {
            viceCol = col;
        }
    }
    ScanQuery(RM::ScanTarget scanTarget, int col, bool isNull)
    {
        queryType = SINGLE;
        scanType = NullScan;
        this->isNull = isNull;
        target = scanTarget;
        if(target == MAIN) {
            mainCol = col;
        }
        else if(target == VICE) {
            viceCol = col;
        }
    }
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
    int ResultNum(){return scanResult->size();}
    int GetNextPair(pair<RID, list<RID>> &item);
};

};

#endif MULTISCAN_H
