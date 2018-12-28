//
// Created by AlexFan on 2018/12/28.
//

#include "RecordModule/RecordHandler.h"

namespace RM {

RecordHandler::RecordHandler(int length)
{
    itemLength = length;
    type = new RM::ItemType[length];
    itemLength = new int[length];
    allowNull = new bool[length];
}

RecordHandler::~RecordHandler()
{
    if(type != NULL) {
        delete type;
    }
    if(itemLength != NULL) {
        delete itemLength;
    }
    delete allowNull;
}

int RecordHandler::PrintRecord(const RM_Record &record)
{
    // TODO: Need to be checked
    BufType content = record.GetData();
    int offset = 0;
    for(int i = 0; i < itemNum; i ++) {
        uint item = content[offset];
        if(type[i] == RM::INT) {
            printf("%12d", (int)item);
        }
        else if(type[i] == RM::FLOAT) {
            printf("%6.6f", (float)item);
        }
        else if(type[i] == RM::CHAR) {
            int cnt = 0;
            int l = (itemLength[i] % 4) ? itemLength[i]/4 + 1 : itemLength[i]/4;
            for(int k = 0; k < l && cnt < itemLength[i]; k ++)
            {
                uint ctx = content[offset + k];
                uint mask = 255;
                for(int shift = 0; shift < 32 && cnt < itemLength[i]; shift ++)
                {
                    char c = (char)((ctx & (mask << shift)) >> shift);
                    printf("%c", c);
                    cnt ++;
                }
            }
        }
        if(i != itemNum - 1) {
            printf("|");
        }
        else{
            printf("\n");
        }
        offset += itemLength[i];
    }
}

bool RecordHandler::isValidChar(uint c)
{
	if(c >= 32 && c <= 124)
		return true;
	else
		return false;
}

int RecordHandler::SetNullInfo(bool *nullInfo, int length)
{
    if(length != itemNum) {
    	return 1;
	}
    for(int i = 0; i < length; i ++) {
    	allowNull[i] = nullInfo[i];
	}
	return 0;
}

int RecordHandler::SetType(int pos, RM::ItemType tp) {
    if(pos >= itemNum || pos < 0) {
        return 1;
    }
    this->itemLength[pos] = tp;
    return 0;
}

int RecordHandler::SetItemLength(int pos, int _length)
{
    if(pos >= itemNum) {
        return 1;
    }
    itemLength[pos] = _length;
    return 0;
}

int RecordHandler::IsAllowNull(int pos)
{
    if(pos < 0 || pos >= itemNum)   {
        printf("Overflow in null info section.\n");
        return 0;
    }
    return allowNull[pos] ? 1 : 0;
}

int RecordHandler::MakeRecord(RM_Record &record, vector<RM_node> &items, RID rid)
{
    if(items.size() != this->itemNum)   {
        return 1;
    }
    int bufSize = 0;
    for(int i = 0; i < this->itemNum; i ++) {
        bufSize += items[i].length;
    }
    int nullSectLength = (itemNum % 32) ? itemNum/32 + 1 : itemNum/32;
    bufSize += nullSectLength;
    BufType buf = new uint[bufSize];
	for(int i = 0, cnt = 0; i < nullSectLength && cnt < itemNum; i ++)
	{
		uint curNum = 0;
		for(int shift = 0; shift < 32 && cnt < itemNum; shift ++)
		{
			curNum += (this->IsAllowNull(cnt) << shift);
			cnt ++;
		}
		buf[i] = curNum;
	}

    int cnt = nullSectLength;
	for(int i = 0; i < itemNum && cnt < bufSize; i ++)
	{
        if(items[i].type == RM::INT || items[i].type == RM::FLOAT) {
            buf[cnt] = items[i].ctx[0];
            cnt ++;
        }
        else if(items[i].type == RM::CHAR) {
            for(int k = 0; k < items[i].length; k ++) {
                buf[cnt] = items[i].ctx[k];
                cnt ++;
            }
        }
        else {
            cout << "Invalid type" << endl;
        }
    }
    record.SetRecord(buf, itemNum, rid);
    return 0;
}

}
