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
    RM::ItemType *type;
    int *itemLength; // 如果item是CHAR类型的，有几个Char，INT、FLOAT型为1（个uint）无意义
    bool *allowNull;
    int nullSectLength; // 记录NULL信息的区段占据了多少个uint
    int recordSize; // 一条record总共占多少uint
public:
    int itemNum;
    bool isInitialized; // 为真时表示在FileHandle的init前就已经设定好值了
    RecordHandler(int length);
    bool isValidChar(uint c);
    ~RecordHandler();
    int PrintRecord(RM_Record &record);
    int GetColumn(int pos, RM_Record &record, RM_node &result);
    int SetType(int pos, RM::ItemType tp);
    RM::ItemType* GetItemType() const {return type;}
    int* GetItemLength() const {return itemLength;}
    int SetItemLength(int pos, int _length);
    int SetNullInfo(bool *nullInfo, int length);
    int SetRecordSize(int size);
    int IsAllowNull(int pos);
    int SetItemAttribute(int pos, int length, RM::ItemType itemType, bool isNull);
    int GetRecordSize() const {return recordSize;}
    int MakeRecord(RM_Record &record, vector<RM_node> &items);
};

}

#endif //RM_RECORD_HANDLER_H
