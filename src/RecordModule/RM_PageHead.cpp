#include "../include/RecordModule/RM_PageHead.h"
#include <iostream>
using namespace std;
PageHead::PageHead(){
}

PageHead::PageHead(int rl, int rpp, int rsum, int cNum){
    buf = new uint[PAGE_INT_NUM];
    memset(buf, 0, PAGE_INT_NUM);
    buf[0] = rl;//record length
    buf[1] = rpp;//record per page
    cout<<"rpp"<<rpp<<endl;
    buf[2] = rsum;//record sum
    buf[3] = 1;//page sum
    buf[4] = cNum;
    for(int i = 5;i < PAGE_INT_NUM;i++)
    	buf[i] = 0xffffffff;
}

BufType PageHead::encode2Buf(){
    return buf;
}
