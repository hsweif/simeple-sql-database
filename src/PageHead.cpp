#include "../include/RecordModule/PageHead.h"

PageHead::PageHead(){
}

PageHead::PageHead(int rl, int rpp, int rsum){
    buf = new uint[PAGE_INT_NUM];
    memset(buf, 0, PAGE_INT_NUM);
    buf[0] = rl;//record length
    buf[1] = rpp;//record per page
    buf[2] = rsum;//record sum
    buf[3] = 1;//page sum
    for(int i = 4;i < PAGE_INT_NUM;i++)
    	buf[i] = 0xffffffff;
}

BufType PageHead::encode2Buf(){
    return buf;
}
