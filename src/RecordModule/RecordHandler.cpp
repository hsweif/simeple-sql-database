//
// Created by AlexFan on 2018/12/28.
//

#include "RecordModule/RecordHandler.h"

namespace RM {

/**
 * Initialize Function
 * @param length The number of column per record in this table.
 */
RecordHandler::RecordHandler(int length)
{
    isInitialized = false;
    itemNum = length;
    nullSectLength = (itemNum % 32) ? itemNum/32 + 1 : itemNum/32;
    type = new RM::ItemType[length];
    itemLength = new int[length];
    allowNull = new bool[length];
    recordSize = nullSectLength;
}

RecordHandler::~RecordHandler()
{
    if(type != NULL) {
        delete type;
    }
    if(itemLength != NULL) {
        delete itemLength;
    }
    if(allowNull != NULL) {
        delete allowNull;
    }
}

string RecordHandler::GetSplitLine(int i)
{
    string splitLine = "";
    int times = (type[i] == RM::CHAR && itemLength[i] > ALIGN_WIDTH) ? itemLength[i] : ALIGN_WIDTH;
    for(int i = 0; i < times; i ++) {
        splitLine += "-";
    }
    splitLine += "+";
    return splitLine;
}

int RecordHandler::PrintRecord(RM_Record &record)
{
    cout << "|";
    for(int i = 0; i < itemNum; i ++)
    {
        PrintColumn(record, i);
        cout << "|";
    }
    cout << endl;
    string splitLine = "+";
    for(int i = 0; i < itemNum; i ++) {
        splitLine += GetSplitLine(i);
    }
    cout << splitLine << endl;
    return 0;
}

int RecordHandler::PrintRecord(RM_Record &record, vector<int> colIndex)
{
    for(auto iter = colIndex.begin(); iter != colIndex.end(); iter ++)
    {
        if(*iter < 0 || *iter >= itemNum) {
            return 1;
        }
    }
    string splitLine = "+";
    cout << "|";
    for(auto iter = colIndex.begin(); iter != colIndex.end(); iter ++)
    {
        PrintColumn(record, *iter);
        cout << "|";
        splitLine += GetSplitLine(*iter);
    }
    cout << endl;
    cout << splitLine << endl;
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
    if(length != itemNum || isInitialized) {
    	return 1;
	}
    for(int i = 0; i < length; i ++) {
    	allowNull[i] = nullInfo[i];
	}
	return 0;
}

int RecordHandler::SetType(int pos, RM::ItemType tp) {
    if(pos >= itemNum || pos < 0 || isInitialized) {
        return 1;
    }
    this->type[pos] = tp;
    return 0;
}

int RecordHandler::SetItemLength(int pos, int _length)
{
    if(pos >= itemNum || pos < 0 || isInitialized) {
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
    // 做NULL和主键性质的确认

    for(int i = 0; i < this->itemNum; i ++) {
        bool n = items[i].isNull;
        if(items[i].isNull && !this->IsAllowNull(i)){
            cout << "[ERROR] Fail to make record because found null value in a non-null column." << endl;
            return 1;
        }
        if(items[i].isNull) {
            // HINT: 因为如果使用new RM_node() 初始化的null点，是没有type的，语义上还是让他和应有的type一致。
            items[i].type = this->type[i];
        }
    }

    for(int i = 0; i < this->itemNum; i ++) {
        if(items[i].type != type[i]) {
            cout << "Record's attribute type is invalid." << endl;
            return 1;
        }
        if(items[i].type == RM::ItemType::CHAR && !items[i].isNull)
        {
            if(items[i].length > itemLength[i]) {
                cout << "Record's char attribute is too long." << endl;
                return 1;
            }
        }
    }

    int bufSize = 0;
    for(int i = 0; i < this->itemNum; i ++) {
        int tmp = 0;
        if(type[i] == RM::CHAR) {
            int l = itemLength[i];
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
    memset(buf, 0, sizeof(uint)*bufSize);
	for(int i = 0, cnt = 0; i < nullSectLength && cnt < itemNum; i ++)
	{
		uint curNum = 0;
		for(int shift = 0; shift < 32 && cnt < itemNum; shift ++)
		{
			curNum += (items[cnt].isNull << shift);
			cnt ++;
		}
		buf[i] = curNum;
	}

    int cnt = nullSectLength;
	for(int i = 0; i < itemNum && cnt < bufSize; i ++)
	{
        if(items[i].type == RM::INT){
            if(!items[i].isNull) {
                buf[cnt] = items[i].ctx[0];
            }
            else{
                buf[cnt] = 0;
            }
            cnt ++;
        }
        else if(items[i].type == RM::FLOAT) {
            if(!items[i].isNull) {
                buf[cnt] = RM::castFloatToUint(items[i].fNum);
            }
            else{
                buf[cnt] = 0;
            }
            cnt ++;
        }
        else if(items[i].type == RM::CHAR) {
            int tmp_l = (itemLength[i] % 4) ? itemLength[i]/4 + 1 : itemLength[i]/4;
            int item_l = (items[i].length % 4) ? items[i].length/4 + 1 : items[i].length/4;
            for(int k = 0; k < tmp_l; k ++) {
                if(!items[i].isNull && k < item_l) {
                    buf[cnt] = items[i].ctx[k];
                }
                else{
                    buf[cnt] = 0;
                }
                cnt ++;
            }
        }
        else {
            cout << "Invalid type" << endl;
        }
    }
    // Because cnt+1 = bufSize
    record.SetRecord(buf, bufSize, itemNum);
	delete[] buf;
    return 0;
}

int RecordHandler::GetColumn(int pos, RM_Record &record, RM_node &result)
{
    if(pos < 0 || pos >= itemNum) {
        return 1;
    }
    BufType ctx = record.GetData();
    result.isNull = record.IsNull(pos);
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
        float f = RM::castUintToFloat(ctx[offset]);
        result.setCtx(f);
    }
    else if(type[pos] == RM::CHAR)
    {
        // TODO
        string str = "";
        int shift = 0, mask = 255, cnt = 0;
        for(int k = 0; k < l; k ++)
        {
            uint tmp = (uint)((ctx[cnt+offset] & (mask << shift)) >> shift);
            uint tt = ctx[cnt+offset];
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
        result.length = (int)str.length();
    }
    return 0;
}

int RecordHandler::SetColumn(int pos, RM_Record &record, RM_node &input){
    if(pos < 0 || pos >= itemNum) {
        return 1;
    }
    if(input.isNull){
        record.SetNull(pos);
    } 
    int l = itemLength[pos];
    int offset = this->itemNum%32 ? this->itemNum/32+1 : this->itemNum/32;
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
        record.SetRecord(offset,(input.getCtx())[0]);
    }
    else if(type[pos] == RM::FLOAT)
    {
        record.SetRecord(offset,(input.getCtx())[0]);
    }
    else if(type[pos] == RM::CHAR)
    {
        string cStr = input.str;
        int cnt = 0, str_l = cStr.length();
        int uint_l = l % 4 ? l/4 + 1 : l/4;
        for(int k = 0; k < uint_l; k ++)
        {
            uint sum = 0;
            for(int shift = 0; shift < 32; shift += 8)
            {
                if(cnt < str_l)
                {
                    sum += (uint)cStr[cnt] << shift;
                    cnt ++;
                }
            }
            record.SetRecord(k+offset, sum);
            // offset ++;
        }
    }
    return 0;    
}
/**
 * 用来设定每一列的信息,初始化时应该唯一地使用这个方法
 * @param pos 第几列，从0开始
 * @param length 长度，在itemType为CHAR时有意义，代表该栏有多少Char，INT和FLOAT时为1
 * @param itemType 参见RM的枚举，有INT、FLOAT、CHAR三种类型
 * @param isNull 该列是否允许NULL
 * @return
 */
int RecordHandler::SetItemAttribute(int pos, int length, RM::ItemType itemType, bool isNull)
{
    cout << "Set Item Attr" << endl;
    // AWARE
    isInitialized = true;
	if(pos >= itemNum || itemType == RM::ERROR)	{
		return 1;
	}
	if(itemType == RM::CHAR) {
	    int uLength = (length % 4) ? length/4 + 1 : length/4;
	    recordSize += uLength;
    }
    else{
        recordSize ++;
    }

    if(itemType == RM::INT || itemType == RM::FLOAT) {
        itemLength[pos] = 1;
    }
    else{
        itemLength[pos] = length;
    }
	allowNull[pos] = isNull;
	type[pos] = itemType;
	return 0;
}

int RecordHandler::SetRecordSize(int size)
{
    if(!isInitialized) {
        recordSize = size;
        return 0;
    }
    else{
        return 1;
    }
}

/**
 * 打印指定的列的内容，用在select a.name 之类的时候
 * @param record 记录
 * @param col 想打印出记录的那一列的内容
 * @return 0成功
 */
int RecordHandler::PrintColumn(RM_Record &record, int col)
{
    if(col > itemNum || col < 0) {
        return 1;
    }
    bool isNull;
    string colStr;
    string nullStr = "NULL";
    if(this->GetColumnStr(record, col, colStr, isNull)) {
        return 1;
    }
    int width = (type[col] == RM::CHAR && itemLength[col] > ALIGN_WIDTH) ? itemLength[col] : ALIGN_WIDTH;
    if(isNull) {
        cout << setiosflags(ios::left) << setw(width) << nullStr;
    }
    else {
        cout << setiosflags(ios::left) << setw(width) << colStr;
    }
    return 0;
}

int RecordHandler::GetColumnStr(RM_Record &record, int col, string &colStr, bool &isNull)
{
    if(col > itemNum || col < 0) {
        return 1;
    }
    RM_node node;
    this->GetColumn(col, record, node);
    if(node.isNull) {
        colStr = "";
        isNull = true;
    }
    else
    {
        isNull = false;
        stringstream ss;
        if(node.type == RM::INT) {
            ss << node.num;
            ss >> colStr;
        }
        else if(node.type == RM::FLOAT) {
            ss << node.fNum;
            ss >> colStr;
        }
        else if(node.type == RM::CHAR){
            colStr = node.str;
        }
        else{
            return 1;
        }
    }
    return 0;
}

}
