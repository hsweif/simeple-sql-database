#include "RecordModule/RM_Manager.h"
#include "RecordModule/RM_FileHandle.h"
#include "RecordModule/RecordHandler.h"
#include "RecordModule/RM_FileScan.h"
#include "RecordModule/RM_Record.h"
#include "IndexModule/IndexHandle.h"
#include "utils/MyBitMap.h"
#include "IndexModule/bpt.h"
#include "CommandModule/dataBaseManager.h"
#include "RecordModule/MultiScan.h"
#include "SQLParser.h"
#include "util/sqlhelper.h"
#include <vector>
#include <string>
#include <gtest/gtest.h>
#include<iostream>
using namespace std;
using namespace testing;

// 原先声明在头文件里的全局变量。。。
int current = 0;
int tt = 0;
unsigned char h[61];

char dbName[20] = "NewTesting111";
char chartName1[10] = "chart1";
char chartName2[10] = "chart2";

int SQLParserTest(string query)
{
	// parse a given query
	hsql::SQLParserResult result;
	hsql::SQLParser::parse(query, &result);

	// check whether the parsing was successful
	if (result.isValid()) {
		for (auto i = 0u; i < result.size(); ++i) {
			// Print a statement summary.
			hsql::printStatementInfo(result.getStatement(i));
		}
		return 0;
	} else {
		fprintf(stderr, "Given string is not a valid SQL query.\n");
		fprintf(stderr, "%s (L%d:%d)\n",
				result.errorMsg(),
				result.errorLine(),
				result.errorColumn());
		return -1;
	}
}

TEST(IOTest, stdioRedirect)
{
    string str = "My test";
    std::cout << str;
    testing::internal::CaptureStdout();
    std::cout << str;
    std::string output = testing::internal::GetCapturedStdout();
    ASSERT_EQ(str, output);
    std::cout <<  str << " " << output << endl;
}

TEST(PipelineTest, Delete)
{
    ASSERT_EQ(DropDB(dbName), 0);
}

TEST(PipelineTest, Create) {
    MyBitMap::initConst();
    bool createNewDB = true;
    if (createNewDB) {
        MyBitMap::initConst();
        int ret = CreateDB(dbName);
        ASSERT_EQ(0, ret);
    }
    DIR *dir = UseDB(dbName);
    if (dir == NULL) {
        return;
    }
    RM_Manager *rmg = new RM_Manager(dbName);
    int colNum = 3;
    RM_FileHandle *handler = new RM_FileHandle(false);
    if (createNewDB) {
        handler->recordHandler = new RM::RecordHandler(colNum);
        handler->recordHandler->SetItemAttribute(0, 1, RM::INT, false);
        handler->recordHandler->SetItemAttribute(1, 1, RM::FLOAT, true);
        handler->recordHandler->SetItemAttribute(2, 12, RM::CHAR, false);
        int sz = handler->recordHandler->GetRecordSize();
        vector<string> title;
        title.push_back("id");
        title.push_back("test_float");
        title.push_back("name");
        handler->SetTitle(title);
        rmg->createFile(chartName1, sz, colNum);
        // 在init前面才不会被覆盖
        vector<int> mainKey;
        mainKey.push_back(0);
        handler->SetMainKey(mainKey);
    }
    ASSERT_EQ(rmg->openFile(chartName1, *handler), true);

    // HINT: SetTitle 的同时会生成索引，必须在openFile后（handler需要先init）
    if (createNewDB) {
        handler->InitIndex(true);
    }

    rmg->closeFile(*handler);
    delete handler;
    delete rmg;
}



TEST(PipelineTest, Insert) {
    RM_Manager *rmg = new RM_Manager(dbName);
    RM_FileHandle *handler = new RM_FileHandle();
    ASSERT_EQ(rmg->openFile(chartName1, *handler), true);
    vector<RM_node> items;
    //10 with id and 5 with null value
    printf("-------------These will be inserted-------------\n");
    RM_Record testRecord;
    for (int i = 0; i < 10; i++) {
        items.clear();
        string iStr;
        std::stringstream ss;
        ss << i;
        ss >> iStr;
        float f = (float)i + 0.5f;
        RM_node person_a("NAperson" + iStr);
        RM_node id_a(i);
        RM_node test_f(f);
        items.push_back(id_a);
        items.push_back(test_f);
        items.push_back(person_a);
        RM_Record record;
        ASSERT_EQ(handler->recordHandler->MakeRecord(record, items), 0);
        handler->recordHandler->PrintRecord(record);
        handler->InsertRec(record);
    }

    for (int i = 10; i < 15; i++) {
        items.clear();
        string iStr;
        std::stringstream ss;
        ss << i;
        ss >> iStr;
        RM_node person_a("Nperson" + iStr);
        RM_node id_a(i);
        RM_node null_f;
        items.push_back(id_a);
        items.push_back(null_f);
        items.push_back(person_a);
        RM_Record record;
        ASSERT_EQ(handler->recordHandler->MakeRecord(record, items), 0);
        handler->InsertRec(record);
        handler->recordHandler->PrintRecord(record);
    }
    rmg->closeFile(*handler);

}

TEST(PipelineTest, InvalidInsert)
{
    RM_Manager *rmg = new RM_Manager(dbName);
    RM_FileHandle *handler = new RM_FileHandle();
    ASSERT_EQ(rmg->openFile(chartName1, *handler), true);
    RM_node person_a("IllegalGuy");
    RM_node id_a(0);
    RM_node f;
    vector<RM_node> items;
    items.push_back(id_a);
    items.push_back(f);
    items.push_back(person_a);
    RM_Record record;
    // 重复键值在插入时应该会失败
    ASSERT_EQ(handler->recordHandler->MakeRecord(record, items), 0);
    ASSERT_NE(handler->InsertRec(record), 0);
    // Miao!!!!!!
    RM_node emptyID;
    items[0] = emptyID;
    // 主键为空的记录应该在创建记录时失败
    ASSERT_NE(handler->recordHandler->MakeRecord(record, items), 0);

    // 长度过长的记录应该创建失败
    items.clear();
    RM_node person("ThisNameIsAbsolutelyTooLong"), id(27);
    items.push_back(id);
    items.push_back(f);
    items.push_back(person);
    ASSERT_NE(handler->recordHandler->MakeRecord(record, items), 0);

    items.clear();
    RM_node f1(0.24f);
    items.push_back(person_a);
    items.push_back(id);
    items.push_back(f1);
    // 类型错误的记录应该创建失败
    testing::internal::CaptureStdout();
    ASSERT_NE(handler->recordHandler->MakeRecord(record, items), 0);
    std::string output = testing::internal::GetCapturedStdout();
    ASSERT_EQ("Record's attribute type is invalid.\n", output);
    delete handler;
    delete rmg;
}

TEST(PipelineTest, SearchForUniqueRangeCondition) {
    RM_Manager *rmg = new RM_Manager(dbName);
    RM_FileHandle *handler = new RM_FileHandle();
    ASSERT_EQ(rmg->openFile(chartName1, *handler), true);
    RM_FileScan *fileScan = new RM_FileScan;
    RM_Record nextRec;
    printf("-------------List all records with id < 5--------------\n");
    fileScan->OpenScan(*handler, 0, IM::LS, "5");
    int cnt = 0;
    while (!fileScan->GetNextRec(*handler, nextRec)) {
        handler->recordHandler->PrintRecord(nextRec);
        cnt++;
    }
    ASSERT_EQ(cnt, 5);
    fileScan->CloseScan();
    cnt = 0;
    printf("-------------List all records with id >= 5--------------\n");
    fileScan->OpenScan(*handler, 0, IM::GEQ, "5");
    while (!fileScan->GetNextRec(*handler, nextRec)) {
        handler->recordHandler->PrintRecord(nextRec);
        cnt++;
    }
    ASSERT_EQ(cnt, 10);
    fileScan->CloseScan();
    rmg->closeFile(*handler);
    delete handler;
    delete rmg;
    delete fileScan;
}

TEST(PipelineTest, SearchForUniqueNullCondition) {
    RM_Manager *rmg = new RM_Manager(dbName);
    RM_FileHandle *handler = new RM_FileHandle();
    ASSERT_EQ(rmg->openFile(chartName1, *handler), true);
    RM_FileScan *fileScan = new RM_FileScan;
    RM_Record nextRec;
    printf("-------------List all records with null float--------------\n");
    fileScan->OpenScan(*handler, 1, true);
    int cnt = 0;
    while (!fileScan->GetNextRec(*handler, nextRec)) {
        handler->recordHandler->PrintRecord(nextRec);
        cnt++;
    }
    ASSERT_EQ(cnt, 5);
    fileScan->CloseScan();

    printf("-------------List all records with non-null id--------------\n");
    fileScan->OpenScan(*handler, 1, false);
    cnt = 0;
    while (!fileScan->GetNextRec(*handler, nextRec)) {
        handler->recordHandler->PrintRecord(nextRec);
        cnt++;
    }
    ASSERT_EQ(cnt, 10);
    fileScan->CloseScan();
    rmg->closeFile(*handler);
    delete handler;
    delete rmg;
    delete fileScan;
}

TEST(PipelineTest, NestSearch)
{
    RM_Manager *rmg = new RM_Manager(dbName);
    RM_FileHandle *handler = new RM_FileHandle();
    ASSERT_EQ(rmg->openFile(chartName1, *handler), true);
    RM_FileScan *fileScan = new RM_FileScan;
    RM_Record nextRec;
    printf("-------------List all records non-null float--------------\n");
    fileScan->OpenScan(*handler, 1, false);
    int cnt = 0;
    while (!fileScan->GetNextRec(*handler, nextRec)) {
        handler->recordHandler->PrintRecord(nextRec);
        cnt++;
    }
    ASSERT_EQ(cnt, 10);
    printf("-------------List all records with non-null float and id <= 8--------------\n");
    fileScan->OpenScan(*handler, 0, IM::LEQ, "8");
    cnt = 0;
    while (!fileScan->GetNextRec(*handler, nextRec)) {
        handler->recordHandler->PrintRecord(nextRec);
        cnt++;
    }
    ASSERT_EQ(cnt, 9);
    printf("-------------List all records with non-null float and 5 < id <= 8--------------\n");
    fileScan->OpenScan(*handler, 0, IM::GT, "5");
    cnt = 0;
    while (!fileScan->GetNextRec(*handler, nextRec)) {
        handler->recordHandler->PrintRecord(nextRec);
        cnt++;
    }
    ASSERT_EQ(cnt, 3);
    fileScan->CloseScan();

    printf("-------------List all records with null float and 13 > id > 4--------------\n");
    fileScan->OpenScan(*handler, 1, true);
    fileScan->OpenScan(*handler, 0, IM::GT, "5");
    fileScan->OpenScan(*handler, 0, IM::LS, "13");
    cnt = 0;
    while (!fileScan->GetNextRec(*handler, nextRec)) {
        handler->recordHandler->PrintRecord(nextRec);
        cnt++;
    }
    ASSERT_EQ(cnt, 3); // 10, 11, 12
    fileScan->CloseScan();

    rmg->closeFile(*handler);
    delete handler;
    delete rmg;
    delete fileScan;
}

TEST(PipelineTest, ChartConnect)
{
    RM_Manager *rmg = new RM_Manager(dbName);
    RM_FileHandle *mainHandler = new RM_FileHandle();
    RM_FileHandle *viceHandler = new RM_FileHandle(false);
    int colNum = 2;
    viceHandler->recordHandler = new RM::RecordHandler(colNum);
    viceHandler->recordHandler->SetItemAttribute(0, 1, RM::INT, false);
    viceHandler->recordHandler->SetItemAttribute(1, 12, RM::CHAR, false);
    int sz = viceHandler->recordHandler->GetRecordSize();
    vector<string> title;
    title.push_back("id");
    title.push_back("name");
    rmg->createFile(chartName2, sz, colNum);
    viceHandler->SetTitle(title);
    ASSERT_EQ(rmg->openFile(chartName1, *mainHandler), true);
    ASSERT_EQ(rmg->openFile(chartName2, *viceHandler), true);
    viceHandler->InitIndex(true);
    vector<RM_node> items;
    //10 with id and 5 with null value
    for (int i = 0; i < 10; i++) {
        items.clear();
        string iStr;
        std::stringstream ss;
        ss << i;
        ss >> iStr;
        RM_node person_a("elephant" + iStr);
        RM_node id_a(i);
        items.push_back(id_a);
        items.push_back(person_a);
        RM_Record record;
        ASSERT_EQ(viceHandler->recordHandler->MakeRecord(record, items), 0);
        viceHandler->InsertRec(record);
    }
    list<RM::ScanQuery> queryList;
    vector<RM_Record> result;
    viceHandler->GetAllRecord(result);
    ASSERT_EQ(result.size(), 10);
    RM::ScanQuery sQuery(0, IM::EQ, 0);
    queryList.push_back(sQuery);
    RM::DualScan *dualScan = new RM::DualScan(mainHandler, viceHandler);
    dualScan->OpenScan(queryList);
    int cnt = 0;
    pair<RID, list<RID>> item;
    while(!dualScan->GetNextPair(item)) {
        RID mainID = item.first;
        list<RID> viceList = item.second;
        RM_Record mRecord, vRecord;
        mainHandler->GetRec(mainID, mRecord);
        mainHandler->recordHandler->PrintRecord(mRecord);
        for(auto iter = viceList.begin(); iter != viceList.end(); iter ++) {
            RID vRid = *iter;
            viceHandler->GetRec(vRid, vRecord);
            viceHandler->recordHandler->PrintRecord(vRecord);
        }
        cnt ++;
    }
    ASSERT_EQ(cnt, 10);

    string chartName(chartName1);
    ASSERT_EQ(viceHandler->AddForeignKey(rmg, chartName, "id", 0), 0);
    pair<string, int> fkeyInfo;
    ASSERT_EQ(viceHandler->GetForeignKeyInfo(0, fkeyInfo), 1);
    ASSERT_EQ(fkeyInfo.first, chartName);
    ASSERT_EQ(fkeyInfo.second, 0);
    ASSERT_EQ(viceHandler->GetForeignKeyInfo(1, fkeyInfo), 0);

    RM_node id1(1000), id2(0);
    RM_node node1("test"), node2("test1");
    items.clear();
    items.push_back(id1);
    items.push_back(node1);
    RM_Record record;
    ASSERT_EQ(viceHandler->recordHandler->MakeRecord(record, items), 0);
    ASSERT_EQ(viceHandler->InsertRec(record), 1);
    items.clear();
    items.push_back(id2);
    items.push_back(node2);
    ASSERT_EQ(viceHandler->recordHandler->MakeRecord(record, items), 0);
    RID rid(1,0);
    ASSERT_EQ(viceHandler->DeleteRec(rid), 1);
    rmg->closeFile(*mainHandler);
    rmg->closeFile(*viceHandler);
}

TEST(PipelineTest, UpdateRecord)
{
    RM_Manager *rmg = new RM_Manager(dbName);
    RM_FileHandle *handler = new RM_FileHandle();
    ASSERT_EQ(rmg->openFile(chartName1, *handler), true);
    RM_node forTest(233), forTestStr("alexf"), forTestFloat(7.8f);
    RM_node testResult;
    RM_Record testRecord;
    RID rid(1,0);
    ASSERT_EQ(handler->GetRec(rid, testRecord), 0);
    ASSERT_EQ(handler->recordHandler->SetColumn(0, testRecord, forTest), 0);
    ASSERT_EQ(handler->recordHandler->GetColumn(0, testRecord, testResult), 0);
    ASSERT_EQ(forTest == testResult, true);
    ASSERT_EQ(handler->recordHandler->SetColumn(1, testRecord, forTestFloat), 0);
    ASSERT_EQ(handler->recordHandler->GetColumn(1, testRecord, testResult), 0);
    ASSERT_EQ(forTestFloat == testResult, true);
    ASSERT_EQ(handler->recordHandler->SetColumn(2, testRecord, forTestStr), 0);
    ASSERT_EQ(handler->recordHandler->GetColumn(2, testRecord, testResult), 0);
    ASSERT_EQ(forTestStr == testResult, true);
    ASSERT_EQ(handler->UpdateRec(testRecord), 0);
    rmg->closeFile(*handler);
    delete handler;
    delete rmg;
}

TEST(PipelineTest, DeleteNullRecord)
{
    RM_Manager *rmg = new RM_Manager(dbName);
    RM_FileHandle *handler = new RM_FileHandle();
    RM_FileScan *fileScan = new RM_FileScan;
    ASSERT_EQ(rmg->openFile(chartName1, *handler), true);
    RM_Record nextRec;
    fileScan->OpenScan(*handler, 1, true);
    while(fileScan->GetNextRec(*handler, nextRec) != 1) {
        RID rid;
        nextRec.GetRid(rid);
        cout << "The rid to delete is" << rid << endl;
        ASSERT_EQ(handler->DeleteRec(rid), 0);
    }
    vector<RM_Record> result;
    handler->GetAllRecord(result);
    ASSERT_EQ(result.size(), 10);
    rmg->closeFile(*handler);
    fileScan->CloseScan();
    delete handler;
    delete rmg;
    delete fileScan;
}

TEST(PipelineTest, PrintAllRecord)
{
    RM_Manager *rmg = new RM_Manager(dbName);
    RM_FileHandle *handler = new RM_FileHandle();
    ASSERT_EQ(rmg->openFile(chartName1, *handler), true);
    printf("-------------Column Info--------------\n");
    handler->PrintColumnInfo();
    handler->PrintTitle();
    vector<RM_Record> result;
    handler->GetAllRecord(result);
    for(int i = 0; i < result.size(); i ++) {
        handler->recordHandler->PrintRecord(result[i]);
    }
    ASSERT_EQ(result.size(), 10);
    rmg->closeFile(*handler);
    delete handler;
    delete rmg;
}




TEST(PipelineTest, ForeignKey)
{
    RM_Manager *rmg = new RM_Manager(dbName);
    RM_FileHandle *mainHandler = new RM_FileHandle();
    RM_FileHandle *viceHandler = new RM_FileHandle();
    pair<string, int> fkeyInfo;
    ASSERT_EQ(rmg->openFile(chartName1, *mainHandler), true);
    ASSERT_EQ(rmg->openFile(chartName2, *viceHandler), true);
    ASSERT_EQ(viceHandler->GetForeignKeyInfo(0, fkeyInfo), 1);
    string chartName(chartName1);
    ASSERT_EQ(fkeyInfo.first, chartName);
    ASSERT_EQ(fkeyInfo.second, 0);
    ASSERT_EQ(viceHandler->GetForeignKeyInfo(1, fkeyInfo), 0);
    ASSERT_EQ(viceHandler->PrintColumnInfo(), 0);
    rmg->closeFile(*mainHandler);
    rmg->closeFile(*viceHandler);
}

TEST(SQLParserTest, SelectTest)
{
	ASSERT_EQ(0, SQLParserTest("SELECT * FROM test;"));
	ASSERT_EQ(0, SQLParserTest("SELECT name, id FROM test;"));
}

TEST(SQLParseeTest, InsertTest)
{
    ASSERT_EQ(0, SQLParserTest("INSERT INTO test VALUES (1, 2, 3),(4, 5, 3);"));
}
