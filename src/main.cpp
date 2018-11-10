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

    vector<RM_node> vec;
    vector<string> title;
    vector<int> type;
    cout << "Please input 2 titles" << endl;
    string tmp;
    for(int i = 0; i < 2; i ++) {
        cin >> tmp;
        title.push_back(tmp);
        type.push_back(STR_TYPE);
    }
    int recordSize = 10;
    RM_data *data = new RM_data(title, type);
    while(true) {
        vec.clear();
        cout << "Please input 2 items" << endl;
        for(int i = 0; i < 2; i ++) {
            cin >> tmp;
            RM_node node;
            node.setCtx(tmp);
            vec.push_back(node);
        }
        BufType buf = new uint;
        if(data->getSerializeRecord(&buf, vec, recordSize)){
            cout << "error" << endl;
        }
        cout << "new rec " << buf << endl;

        // for(int i = 0; i < recordSize; i ++) {
        //     cout << (uint) buf[i] << " ";
        // }
        vector<RM_node> result; 
        if(data->getRecord(result, buf, recordSize)) {
            cout << "error to get Record" << endl;
        }
        cout << "\nnew result size: " << result.size() << endl;
        vector<string> title1 = data->title();
        cout << "deserialized: " << result.size() <<  endl;
        for(int i = 0; i < result.size(); i ++) {
            cout << title1[i] << ":";
            cout << *(result[i].ctx) << endl;
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
    Test();
    // TestBitMap();
    return 0;
}