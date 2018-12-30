//
// Created by AlexFan on 2018/12/28.
//

#include "RecordModule/RecordHandler.h"

namespace RM {

RecordHandler::RecordHandler(int length)
{
    itemNum = length;
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
            printf("%d", (int)item);
            offset ++;
        }
        else if(type[i] == RM::FLOAT) {
            printf("%f", (float)item);
            offset ++;
        }
        else if(type[i] == RM::CHAR) {
            int cnt = 0;
            int l = (itemLength[i] % 4) ? itemLength[i]/4 + 1 : itemLength[i]/4;
            for(int k = 0; k < l && cnt < itemLength[i]; k ++)
            {
                uint ctx = content[offset + k];
                uint mask = 255;
                for(int shift = 0; shift < 32 && cnt < itemLength[i]; shift += 8)
                {
                    uint tmp = ((ctx & (mask << shift)) >> shift);
                    if(isValidChar(tmp)) {
                        char c = (char)tmp;
                        printf("%c", c);
                    }
                    cnt ++;
                }
            }
            offset += l;
        }
        if(i != itemNum - 1) {
            printf("|");
        }
        else{
            printf("\n");
        }
    }
    return 0;
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
    this->type[pos] = tp;
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

int RecordHandler::MakeRecord(RM_Record &record, vector<RM_node> &items)
{
    if(items.size() != this->itemNum)   {
        return 1;
    }
    int bufSize = 0;
    for(int i = 0; i < this->itemNum; i ++) {
        int tmp = 0;
        if(items[i].type == RM::CHAR) {
            tmp = (itemLength[i] % 4) ? itemLength[i]/4 + 1 : itemLength[i]/4;
        }
        else{
            tmp = 1;
        }
        bufSize += tmp;
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
            int tmp_l = (items[i].length % 4) ? items[i].length/4 + 1 : items[i].length/4;
            for(int k = 0; k < tmp_l; k ++) {
                buf[cnt] = items[i].ctx[k];
                cnt ++;
            }
        }
        else {
            cout << "Invalid type" << endl;
        }
    }
    // Because cnt+1 = bufSize
    record.SetRecord(buf, cnt-nullSectLength+1, itemNum);
    return 0;
}

int RecordHandler::GetColumn(int pos, const RM_Record &record, RM_node &result)
{
    if(pos < 0 || pos >= itemNum) {
        return 1;
    }
    BufType ctx = record.GetData();
    int l = itemLength[pos];
    int offset = 0;
    for(int i = 0; i < pos; i ++) {
        int tmp = 0;
        if(type[i] == RM::INT || type[i] == RM::FLOAT) {
            tmp = 1;
        } else{
            tmp = (itemLength[i] % 4) ? itemLength[i]/4 + 1 : itemLength[i]/4;
        }
        offset += tmp;
    }
    if(type[pos] == RM::INT)
    {
        int n = (int)ctx[offset];
        result.setCtx(n);
    }
    else if(type[pos] == RM::FLOAT)
    {
        float f = (float)ctx[offset];
        result.setCtx(f);
    }
    else if(type[pos] == RM::CHAR)
    {
        // TODO
        string str = "";
        int shift = 0, mask = 255, cnt = 0;
        for(int k = 0; k < l; k ++)
        {
            uint tmp = (uint)((ctx[cnt] & (mask << shift)) >> shift);
            if(!isValidChar(tmp)) {
                break;
            }
            char c = (char)tmp;
            if(shift == 24) {
                shift = 0;
                cnt ++;
            }
            else {
                shift += 8;
            }
            str += c;
        }
        result.setCtx(str);
        result.length = l;
    }
    return 0;
}


int RecordHandler::SetItemAttribute(int pos, int length, RM::ItemType itemType, bool isNull)
{
	if(pos >= itemNum)	{
		return 1;
	}
	if((itemType == RM::INT || itemType == RM::FLOAT) && length != 1) {
		return 1;
	}
	itemLength[pos] = length;
	allowNull[pos] = isNull;
	type[pos] = itemType;
	return 0;
}

}
