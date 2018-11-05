#include "../include/RecordModule/RM_data.h"

RM_data::RM_data(){
    length = 0;
}

RM_data::RM_data(std::vector<string> title, int itl) {
    arr = title;
    length = arr.size();
    itemLength = itl;
}

int RM_data::dataLenth(){
    return length == 0 ? arr.size() : length;
}

BufType RM_data::getSerializeRecord(std::vector<string> data, int &recordSize) {
    recordSize = (data.size() * itemLength)/4 + 1;
    BufType buf = new uint[recordSize]; 
    int l = data.size();
    int cnt = 0;
    int offset = 0; //因为一个uint可以存四个char
    for(int i = 0; i < l; i ++) {
        for(int j = 0; j < itemLength; j ++) {
            if(j < data[i].length()) {
                buf[cnt] += (uint)data[i][j] << offset;
            }
            if(offset == 24) {
                cnt ++;
                offset = 0;
            }
            else{
                offset += 8;
            }
        }
    }
    return buf;
}

std::vector<string> RM_data::title() {
    return arr;
}

std::vector<string> RM_data::getRecord(BufType serializedBuf, int length) {
    std::vector<string> vec;    
    int cnt = 0;
    int offset = 0;
    uint mask = 255;
    for(int i = 0; i < arr.size(); i ++) {
        char c[itemLength];
        memset(c, 0, sizeof(c));
        for(int j = 0; j < itemLength; j ++) {
            uint tmp = (serializedBuf[cnt] & (mask << offset)) >> offset;
            c[j] = (char)tmp;
            if(offset == 24) {
                cnt ++;
                offset = 0;
            }
            else {
                offset += 8;
            }
        }
        string str(c);
        vec.push_back(str);
    }
    return vec;
}