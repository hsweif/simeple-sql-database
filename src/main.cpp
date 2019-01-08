#include "RecordModule/RM_Manager.h"
#include "RecordModule/RM_FileHandle.h"
#include "RecordModule/RecordHandler.h"
#include "RecordModule/RM_FileScan.h"
#include "RecordModule/RM_Record.h"
#include "IndexModule/IndexHandle.h"
#include "utils/MyBitMap.h"
#include "IndexModule/bpt.h"
#include "CommandModule/dataBaseManager.h"
#include "parser.h"
#include <vector>
#include <string>

// 原先声明在头文件里的全局变量。。。
int current = 0;
int tt = 0;
unsigned char h[61];

using namespace std;


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
	cout << "left" << b->findLeftOne() << endl;
	b->setBit(33, 1);
	cout << "left" << b->findLeftOne() << endl;
	//b->setBit(3, 1);
	b->show();
}

int main() {
#ifdef __DARWIN_UNIX03
	printf("It is on Unix now.\n");
#endif
	MyBitMap::initConst();
	ParseInput("../testcase/test3.txt");
	return 0;
}