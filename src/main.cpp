#include "../include/RecordModule/RM_Manager.h"
#include "../include/RecordModule/RM_data.h"
#include "../include/RecordModule/RM_FileHandle.h"
#include "../include/utils/MyBitMap.h"
#include <vector>
#include <string>

// 原先声明在头文件里的全局变量。。。
int current = 0;
int tt = 0;
unsigned char h[61];

using namespace std;

void Test(){
    // RM_Manager *rmg = new RM_Manager();
    // RM_FileHandle *handler = new RM_FileHandle();
    // rmg->createFile("test", 10);
    // string test = rmg->openFile("test", *handler) ? "successfully opened" : "fail to open";
    // cout << test << endl;

    vector<string> vec;
    cout << "Please input 3 titles" << endl;
    string tmp;
    for(int i = 0; i < 3; i ++) {
        cin >> tmp;
        vec.push_back(tmp);
    }
    int recordSize = 10;
    RM_data *data = new RM_data(vec, recordSize);
    while(true) {
        vec.clear();
        cout << "Please input 3 items" << endl;
        for(int i = 0; i < 3; i ++) {
            cin >> tmp;
            vec.push_back(tmp);
        }
        BufType buf = data->getSerializeRecord(vec, recordSize);
        for(int i = 0; i < 10; i ++) {
            cout << buf[i] << " ";
        }
        vector<string> result = data->getRecord(buf, recordSize);
        vector<string> title = data->title();
        cout << "deserialized: " << result.size() <<  endl;
        for(int i = 0; i < result.size(); i ++) {
            cout << title[i] << " " << result[i] << endl;
        }
    }
}

void TestBitMap(){
    BufType buf = new uint[10];
    for(int i = 0; i < 10; i ++) {
        buf[i] = 256;
    }

    MyBitMap *b = new MyBitMap(12<<5, 1);
	b->show();
	b->setBit(0,1);
	b->setBit(1,1);
	b->setBit(3,1);
	b->show();
	b->initConst();
	cout<< "Left one: " << b->findLeftOne()<<endl;
}
int main(){
    // Test();
    TestBitMap();
    return 0;
}