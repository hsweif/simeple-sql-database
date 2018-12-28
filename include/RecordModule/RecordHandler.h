//
// Created by AlexFan on 2018/12/28.
//


#ifndef RM_RECORD_HANDLER_H
#define RM_RECORD_HANDLER_H

#include <iostream>
#include <vector>
#include <string>
#include "RM_Record.h"
#include "RID.h"

using namespace std;
namespace RM {

class RecordHandler
{
private:
    int itemNum;
    int recordSize; // 一条record占多少uint
    RM::ItemType *type;
    int *itemLength;
    bool *allowNull;
public:
    RecordHandler(int length);
    bool isValidChar(uint c);
    ~RecordHandler();
    void PrintRecord(const RM_Record &record);
    int GetColumn(int pos, const RM_Record &record, RM_node &result);
    int SetType(int pos, RM::ItemType tp);
    RM::ItemType* GetItemType() const {return type;}
    int* GetItemLength() const {return itemLength;}
    int SetItemLength(int pos, int _length);
    int SetNullInfo(bool *nullInfo, int length);
    int IsAllowNull(int pos);
    int MakeRecord(RM_Record &record, vector<RM_node> &items, RID rid);
};

}

#endif //RM_RECORD_HANDLER_H
