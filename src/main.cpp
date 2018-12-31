#include "RecordModule/RM_Manager.h"
#include "RecordModule/RM_FileHandle.h"
#include "RecordModule/RecordHandler.h"
#include "RecordModule/RM_FileScan.h"
#include "RecordModule/RM_Record.h"
#include "IndexModule/IndexHandle.h"
#include "utils/MyBitMap.h"
#include "IndexModule/bpt.h"
#include "CommandModule/dataBaseManager.h"
#include <vector>
#include <string>

// 原先声明在头文件里的全局变量。。。
int current = 0;
int tt = 0;
unsigned char h[61];

using namespace std;

void NewTest(bool createNewDB, char *dbName)
{
    string mode = createNewDB ? "Create new database" : "Use currently data base";
    cout << mode << endl;
	if(createNewDB) {
        CreateDB(dbName);
	}
	DIR *dir = UseDB(dbName);
	if(dir == NULL) {
		cout << "Error in opening database." << endl;
		return;
	}
	RM_Manager *rmg = new RM_Manager(dbName);
	int colNum = 2;

	RM_FileHandle *handler = new RM_FileHandle();
	if(createNewDB)
	{
        handler->recordHandler = new RM::RecordHandler(colNum);
        handler->recordHandler->SetItemAttribute(0, 8, RM::CHAR, false);
        handler->recordHandler->SetItemAttribute(1, 1, RM::INT, true);
        int sz = handler->recordHandler->GetRecordSize();
		vector<string> title;
		title.push_back("name");
		title.push_back("id");
		handler->SetTitle(title);
        rmg->createFile(dbName, sz, colNum, *handler);
		handler->InitIndex(true);
	}

	string test = rmg->openFile(dbName, *handler) ? "successfully opened" : "fail to open";
	cout << test << endl;

	// 在init后面才不会被覆盖
	// handler->SetMainKey(1);

	// HINT: SetTitle 的同时会生成索引，必须在openFile后（handler需要先init）
	if(createNewDB) {
        vector<RM_node> items;
        for(int i = 0; i < 10; i ++) {
            items.clear();
            RM_node person_a("person_test");
            RM_node id_a(i/2);
            items.push_back(person_a);
            items.push_back(id_a);
            RM_Record record;
            if(handler->recordHandler->MakeRecord(record, items)) {
                cout << "Error to make record." << endl;
            }
            handler->recordHandler->PrintRecord(record);
            handler->InsertRec(record);
        }

	}

	printf("Searched result for records with id between %d and %d\n", 2, 8);
	vector<RID> rid;
	handler->indexHandle->SearchRange(rid, "2", "8", IM::LS, 1);
	for(int i = 0; i < rid.size(); i ++) {
		cout << rid[i] << endl;
	}

	printf("-------------List all records in the file--------------\n");
	handler->PrintTitle();
	vector<RM_Record> result;
	handler->GetAllRecord(result);
	for(int i = 0; i < result.size(); i ++) {
		handler->recordHandler->PrintRecord(result[i]);
	}
	rmg->closeFile(*handler);
}
/*
void test1(){
    RM_Manager *rmg = new RM_Manager("test");
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
	rmg->closeFile(*handler);
}
*/

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
#ifdef __DARWIN_UNIX03
    printf("It is on Unix now.\n");
#endif
    MyBitMap::initConst();
	char *dbName = "NewTesting1_3";
	NewTest(true, dbName);
    NewTest(false, dbName);
    return 0;
}