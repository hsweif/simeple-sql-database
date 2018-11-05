#include "../include/RecordModule/PageHead.h"

PageHead::PageHead(){
}

PageHead::PageHead(int rl, int rpp, int rsum){
    buf = new uint[PAGE_INT_NUM];
    memset(buf, 0, PAGE_INT_NUM);
    buf[0] = rl;
    buf[1] = rpp;
    buf[2] = rsum;
}

BufType PageHead::encode2Buf(){
    return buf;
}