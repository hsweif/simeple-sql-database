#include "../include/RecordModule/RM_Manager.h"
#include "../include/RecordModule/RM_data.h"
#include "../include/RecordModule/RM_FileHandle.h"
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

void test1(){
    RM_Manager *rmg = new RM_Manager();
    RM_FileHandle *handler = new RM_FileHandle();
    rmg->createFile("helloworld1", 500);
    string test = rmg->openFile("helloworld1", *handler) ? "successfully opened" : "fail to open";
    BufType buf = new uint[500];
	for(int i = 0;i < 500;i++)
		buf[i] = 0;
    RM_Record pData;
	RID insertId(1, 0);
	RID deleteId(1, 0);
    pData.SetRecord(buf,500,insertId);
    handler->InsertRec(pData);
	//cout << endl;
	handler->InsertRec(pData);
	//cout << endl;
	handler->InsertRec(pData);
	//cout << endl;
	handler->InsertRec(pData);
	//cout << endl;
	handler->DeleteRec(deleteId);
	//cout << endl;
	handler->InsertRec(pData);
	//cout << endl;
	handler->InsertRec(pData);
	//cout << endl;
    //cout << test << endl;
    //handler->show();
	rmg->closeFile(*handler);
}

void testBitmap() {
	
	MyBitMap *b = new MyBitMap(2 << 5, 2);
	b->show();
	cout << "left"<<b->findLeftOne() << endl;
	b->setBit(33, 1);
	cout << "left" << b->findLeftOne() << endl;
	//b->setBit(3, 1);
	b->show();
}
int main(){
	MyBitMap::initConst();
    test1();
    return 0;
}