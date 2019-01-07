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

char *dbName = "abctesting12";
vector<RM_Record> orig;

int SQLParserTest(string query)
{
	// parse a given query
	hsql::SQLParserResult result;
	hsql::SQLParser::parse(query, &result);

	// check whether the parsing was successful
	if (result.isValid()) {
		printf("Parsed successfully!\n");
		printf("Number of statements: %lu\n", result.size());
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
        rmg->createFile(dbName, sz, colNum);
        // 在init前面才不会被覆盖
        vector<int> mainKey;
        mainKey.push_back(0);
        handler->SetMainKey(mainKey);
    }
    string test = rmg->openFile(dbName, *handler) ? "successfully opened" : "fail to open";
    ASSERT_EQ(test, "successfully opened");

    // HINT: SetTitle 的同时会生成索引，必须在openFile后（handler需要先init）
    if (createNewDB) {
        handler->InitIndex(true);
    }

    rmg->closeFile(*handler);
}



TEST(PipelineTest, Insert) {
    RM_Manager *rmg = new RM_Manager(dbName);
    RM_FileHandle *handler = new RM_FileHandle();
    string test = rmg->openFile(dbName, *handler) ? "successfully opened" : "fail to open";
    ASSERT_EQ(test, "successfully opened");
    vector<RM_node> items;
    //10 with id and 5 with null value
    printf("-------------These will be inserted-------------\n");
    for (int i = 0; i < 10; i++) {
        items.clear();
        string iStr;
        std::stringstream ss;
        ss << i;
        ss >> iStr;
        float f = (float)i + 0.5f;
        RM_node person_a("person" + iStr);
        RM_node id_a(i);
        RM_node test_f(f);
        items.push_back(id_a);
        items.push_back(test_f);
        items.push_back(person_a);
        RM_Record record;
        if (handler->recordHandler->MakeRecord(record, items)) {
            cout << "Error to make record." << endl;
        }
        handler->recordHandler->PrintRecord(record);
        handler->InsertRec(record);
        orig.push_back(record);
    }
    for (int i = 10; i < 15; i++) {
        items.clear();
        string iStr;
        std::stringstream ss;
        ss << i;
        ss >> iStr;
        RM_node person_a("N_person" + iStr);
        RM_node id_a(i);
        RM_node null_f;
        items.push_back(id_a);
        items.push_back(null_f);
        items.push_back(person_a);
        RM_Record record;
        if (handler->recordHandler->MakeRecord(record, items)) {
            cout << "Error to make record." << endl;
        }
        handler->InsertRec(record);
        handler->recordHandler->PrintRecord(record);
        orig.push_back(record);
    }
    rmg->closeFile(*handler);

}

TEST(PipelineTest, InvalidInsert)
{
    RM_Manager *rmg = new RM_Manager(dbName);
    RM_FileHandle *handler = new RM_FileHandle();
    string test = rmg->openFile(dbName, *handler) ? "successfully opened" : "fail to open";
    ASSERT_EQ(test, "successfully opened");
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
}

TEST(PipelineTest, SearchForUniqueRangeCondition) {
    RM_Manager *rmg = new RM_Manager(dbName);
    RM_FileHandle *handler = new RM_FileHandle();
    string test = rmg->openFile(dbName, *handler) ? "successfully opened" : "fail to open";
    ASSERT_EQ(test, "successfully opened");
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
}

TEST(PipelineTest, SearchForUniqueNullCondition) {
    RM_Manager *rmg = new RM_Manager(dbName);
    RM_FileHandle *handler = new RM_FileHandle();
    string test = rmg->openFile(dbName, *handler) ? "successfully opened" : "fail to open";
    ASSERT_EQ(test, "successfully opened");
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
}

TEST(PipelineTest, NestSearch)
{
    RM_Manager *rmg = new RM_Manager(dbName);
    RM_FileHandle *handler = new RM_FileHandle();
    string test = rmg->openFile(dbName, *handler) ? "successfully opened" : "fail to open";
    ASSERT_EQ(test, "successfully opened");
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
}

TEST(PipelineTest, PrintAllRecord)
{
    RM_Manager *rmg = new RM_Manager(dbName);
    RM_FileHandle *handler = new RM_FileHandle();
    string test = rmg->openFile(dbName, *handler) ? "successfully opened" : "fail to open";
    ASSERT_EQ(test, "successfully opened");
    printf("-------------Column Info--------------\n");
    handler->PrintColumnInfo();
    printf("-------------List all records --------------\n");
    handler->PrintTitle();
    vector<RM_Record> result;
    handler->GetAllRecord(result);
    for(int i = 0; i < result.size(); i ++) {
        printf("---------------------------\n");
        ASSERT_EQ(0, handler->PrintAttribute("id", result[i]));
        cout << "|";
        ASSERT_EQ(0, handler->PrintAttribute("test_float", result[i]));
        cout << "|";
        ASSERT_EQ(0, handler->PrintAttribute("name", result[i]));
        cout << endl;
        handler->recordHandler->PrintRecord(result[i]);
    }
    ASSERT_EQ(result.size(), 15);
    ASSERT_EQ(result.size(), orig.size());
    rmg->closeFile(*handler);
}

TEST(PipelineTest, DeleteRecord)
{
    RM_Manager *rmg = new RM_Manager(dbName);
    RM_FileHandle *handler = new RM_FileHandle();
    ASSERT_EQ(rmg->openFile(dbName, *handler), true);
    RID rid(1, 0);
    handler->DeleteRec(rid);
    vector<RM_Record> result;
    handler->GetAllRecord(result);
    ASSERT_EQ(result.size(), 14);
    rmg->closeFile(*handler);
}

TEST(PipelineTest, ChartConnectAndForeignKey)
{
    char *table1 = "Chart1";
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
    rmg->createFile(table1, sz, colNum);
    viceHandler->SetTitle(title);
    ASSERT_EQ(rmg->openFile(dbName, *mainHandler), true);
    ASSERT_EQ(rmg->openFile(table1, *viceHandler), true);
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
        printf("---------------------------\n");
        mainHandler->recordHandler->PrintRecord(mRecord);
        for(auto iter = viceList.begin(); iter != viceList.end(); iter ++) {
            RID vRid = *iter;
            viceHandler->GetRec(vRid, vRecord);
            viceHandler->recordHandler->PrintRecord(vRecord);
        }
        cnt ++;
    }
    ASSERT_EQ(cnt, 10);

    string chartName(dbName);
    ASSERT_EQ(viceHandler->AddForeignKey(rmg, chartName, "id", 0), 0);
    pair<string, int> fkeyInfo;
    ASSERT_EQ(viceHandler->GetForeignKeyInfo(0, fkeyInfo), 1);
    ASSERT_EQ(fkeyInfo.first, chartName);
    ASSERT_EQ(fkeyInfo.second, 0);
    ASSERT_EQ(viceHandler->GetForeignKeyInfo(1, fkeyInfo), 0);

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
