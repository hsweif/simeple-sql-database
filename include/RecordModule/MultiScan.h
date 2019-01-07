//
// Created by AlexFan on 2019/1/7.
//

#ifndef MULTISCAN_H
#define MULTISCAN_H

#include "RM_FileScan.h"
#include "../config.h"

using namespace std;
namespace RM{

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
    int OpenScan(IM::CompOp compOp, int mCol, int vCol);
    int CloseScan();
};

};

#endif MULTISCAN_H
