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
    int max_cnt = data.size() * itemLength;
    recordSize = max_cnt * sizeof(uint);
    BufType buf = new uint[max_cnt]; 
    int l = data.size();
    int cnt = 0;
    for(int i = 0; i < l; i ++) {
        for(int j = 0; j < itemLength; j ++) {
            if(j < data[i].length()) {
                buf[cnt] = (uint)data[i][j];
            }
            cnt ++;
        }
    }
    return buf;
}

std::vector<string> RM_data::title() {
    return arr;
}

std::vector<string> RM_data::getRecord(BufType serializedBuf, int length) {
    std::vector<string> vec;    
    length /= sizeof(uint);
    int cnt = 0;
    for(int i = 0; i < arr.size(); i ++) {
        char c[itemLength];
        memset(c, 0, sizeof(c));
        for(int j = 0; j < itemLength; j ++) {
            c[j] = (char)serializedBuf[cnt];
            cnt ++;
        }
        string str(c);
        vec.push_back(str);
    }
    return vec;
}