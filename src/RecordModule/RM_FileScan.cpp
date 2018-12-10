#include "RecordModule/RM_FileScan.h"

RM_FileScan::RM_FileScan()
{

}

RM_FileScan::RM_FileScan(vector<int> tp)
{
    this->typeArr = tp;
}

int RM_FileScan::OpenScan(RM_FileHandle &fileHandle, int attrKey, int attrType, int compOp)
{
    int sum = fileHandle.RecordNum();
    int pageCount = fileHandle.PageNum();
    int page = 1, slot = 0;
    int cnt = 0;
    while(cnt < sum)
    {
        RID rid(page, slot);
        RM_Record record;
        if(fileHandle.GetRec(rid, record) == 0)
        {
            // FIXME
            record.SetType(this->typeArr);
            record.Print();
            slot ++;
            cnt ++;
        }
    }

}

int RM_FileScan::GetNextRec(RM_Record &rec)
{

}

int RM_FileScan::CloseScan()
{

}
