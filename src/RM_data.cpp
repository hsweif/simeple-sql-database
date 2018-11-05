#include "../include/RecordModule/RM_data.h"

RC_data::RC_data(){
    length = 0;
}

RC_data::RC_data(std::vector<char*> title, int itl) {
    arr = title;
    length = arr.size();
    itemLength = itl;
}

int RC_data::dataLenth(){
    return length == 0 ? arr.size() : length;
}

int RC_data::getSerializeRecord(std::vector<char*> data, BufType buf, int &recordSize) {
    if(data.size() != dataLenth()) {
        return 1;
    }
    else {
        int max_cnt = data.size() * itemLength;
        recordSize = max_cnt * sizeof(uint);
        buf = new uint[max_cnt]; 
        int l = data.size();
        int cnt = 0;
        for(int i = 0; i < l; i ++) {
            for(int j = 0; j < itemLength; j ++) {
                if(data[i][j]) {
                    buf[cnt] = (uint)data[i][j];
                }
                cnt ++;
            }
        }
        return 0;
    }
}

std::vector<char*> RC_data::title() {
    return arr;
}

std::vector<char*> RC_data::getRecord(BufType serializedBuf) {
    std::vector<char*> vec;    
    int cnt = 0;
    for(int i = 0; i < vec.size(); i ++) {
        char c[itemLength];
        for(int j = 0; j < itemLength; j ++) {
            c[j] = (char)serializedBuf[cnt];
            cnt ++;
        }
        vec.push_back(c);
    }
    return vec;
}