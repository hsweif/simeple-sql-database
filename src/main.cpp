#include "../include/RecordModule/RM_Manager.h"

// 原先声明在头文件里的全局变量。。。
int current = 0;
int tt = 0;
unsigned char h[61];

int main(){
    RM_Manager *rmg = new RM_Manager();
    rmg->createFile("test1", 10);
    string test = rmg->openFile("test1") ? "successfully opened" : "fail to open";
    cout << test << endl;
    return 0;
}