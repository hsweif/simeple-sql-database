#include "RecordModule/RM_Manager.h"
#include "RecordModule/RM_data.h"
#include "RecordModule/RM_FileHandle.h"
#include "utils/MyBitMap.h"
#include <vector>
#include <string>

// 原先声明在头文件里的全局变量。。。
int current = 0;
int tt = 0;
unsigned char h[61];

using namespace std;

void Test(){
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
    RM_Manager *rmg = new RM_Manager();
    RM_FileHandle *handler = new RM_FileHandle();
    int recordSize = 2 * ITEM_LENGTH/4;
    rmg->createFile("helloworld2", recordSize);
    string test = rmg->openFile("helloworld2", *handler) ? "successfully opened" : "fail to open";
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
        cout << "recordSize" << recordSize << endl;
        if(data->getSerializeRecord(&buf, vec, recordSize)){
            cout << "error" << endl;
        }
        cout << "new rec " << buf << endl;
        RM_Record pData, nData;
        pData.SetRecord(buf, recordSize, RID(1,0));
        RID rid;
        if(pData.GetRid(rid))
            cout << "error to get rid" << endl;
        handler->InsertRec(pData);
        handler->InsertRec(pData);
        handler->InsertRec(pData);
        handler->InsertRec(pData);
        handler->InsertRec(pData);
        handler->InsertRec(pData);
        handler->GetRec(rid, nData);
        cout << "gotr rec " << nData.GetData() << endl;
        vector<RM_node> result; 
        if(data->getRecord(result, nData.GetData(), recordSize)) {
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

void test1(){
    RM_Manager *rmg = new RM_Manager();
    RM_FileHandle *handler = new RM_FileHandle();
    rmg->createFile("helloworld1", 500);
    string test = rmg->openFile("helloworld1", *handler) ? "successfully opened" : "fail to open";
    BufType buf = new uint[500];
    for(int i = 0;i < 500;i++)
    	buf[i] = i;
    BufType upBuf = new uint[500];
    for (int i = 0; i < 500; i++)
    	upBuf[i] = 0;
    RM_Record pData;
	RID insertId(1, 0);//useless
	RID deleteId(1, 0);
	RID queryId(1, 1);
	RID upId(1, 1);
	RM_Record rec;
	RM_Record upRec;
	upRec.SetRecord(upBuf,500, upId);
    pData.SetRecord(buf,500,insertId);
    handler->InsertRec(pData);
	handler->InsertRec(pData);
	handler->InsertRec(pData);
	handler->InsertRec(pData);
	handler->DeleteRec(deleteId);
	handler->InsertRec(pData);
	handler->InsertRec(pData);
	handler->UpdateRec(upRec);
	handler->GetRec(queryId, rec);
	BufType recBuf = rec.GetData();
	//for (int i = 0; i < 500; i++)
	//	cout << recBuf[i] << endl;
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
    // test1();
    Test();
    return 0;
}