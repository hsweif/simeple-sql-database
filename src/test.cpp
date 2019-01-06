#include "RecordModule/RM_Manager.h"
#include "RecordModule/RM_FileHandle.h"
#include "RecordModule/RecordHandler.h"
#include "RecordModule/RM_FileScan.h"
#include "RecordModule/RM_Record.h"
#include "IndexModule/IndexHandle.h"
#include "utils/MyBitMap.h"
#include "IndexModule/bpt.h"
#include "CommandModule/dataBaseManager.h"
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

char *dbName = "UnitTestDB_1";
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
    int colNum = 2;
    RM_FileHandle *handler = new RM_FileHandle();
    if (createNewDB) {
        handler->recordHandler = new RM::RecordHandler(colNum);
        handler->recordHandler->SetItemAttribute(0, 8, RM::CHAR, false);
        handler->recordHandler->SetItemAttribute(1, 1, RM::INT, true);
        int sz = handler->recordHandler->GetRecordSize();
        vector<string> title;
        title.push_back("name");
        title.push_back("id");
        handler->SetTitle(title);
        rmg->createFile(dbName, sz, colNum);
        // 在init前面才不会被覆盖
        vector<int> mainKey;
        mainKey.push_back(1);
        // handler->SetMainKey(mainKey);
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
        RM_node person_a("person");
        RM_node id_a(i);
        items.push_back(person_a);
        items.push_back(id_a);
        RM_Record record;
        if (handler->recordHandler->MakeRecord(record, items)) {
            cout << "Error to make record." << endl;
        }
        handler->recordHandler->PrintRecord(record);
        handler->InsertRec(record);
        orig.push_back(record);
    }
    for (int i = 0; i < 5; i++) {
        items.clear();
        RM_node person_a("N_person");
        RM_node id_a;
        items.push_back(person_a);
        items.push_back(id_a);
        RM_Record record;
        if (handler->recordHandler->MakeRecord(record, items)) {
            cout << "Error to make record." << endl;
        }
        handler->InsertRec(record);
        orig.push_back(record);
    }
    rmg->closeFile(*handler);

}

TEST(PipelineTest, SearchForUniqueRangeCondition) {
    RM_Manager *rmg = new RM_Manager(dbName);
    RM_FileHandle *handler = new RM_FileHandle();
    string test = rmg->openFile(dbName, *handler) ? "successfully opened" : "fail to open";
    ASSERT_EQ(test, "successfully opened");
    RM_FileScan *fileScan = new RM_FileScan;
    RM_Record nextRec;
    printf("-------------List all records with id < 5--------------\n");
    fileScan->OpenScan(*handler, 1, IM::LS, "5");
    int cnt = 0;
    while (!fileScan->GetNextRec(*handler, nextRec)) {
        handler->recordHandler->PrintRecord(nextRec);
        cnt++;
    }
    ASSERT_EQ(cnt, 5);
    fileScan->CloseScan();
    cnt = 0;
    printf("-------------List all records with id > 5--------------\n");
    fileScan->OpenScan(*handler, 1, IM::GT, "5");
    while (!fileScan->GetNextRec(*handler, nextRec)) {
        handler->recordHandler->PrintRecord(nextRec);
        cnt++;
    }
    ASSERT_EQ(cnt, 4);
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
    printf("-------------List all records with null id--------------\n");
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
    printf("-------------List all records non-null id--------------\n");
    fileScan->OpenScan(*handler, 1, false);
    int cnt = 0;
    while (!fileScan->GetNextRec(*handler, nextRec)) {
        handler->recordHandler->PrintRecord(nextRec);
        cnt++;
    }
    ASSERT_EQ(cnt, 10);
    printf("-------------List all records with non-null id <= 8--------------\n");
    fileScan->OpenScan(*handler, 1, IM::LEQ, "8");
    cnt = 0;
    while (!fileScan->GetNextRec(*handler, nextRec)) {
        handler->recordHandler->PrintRecord(nextRec);
        cnt++;
    }
    ASSERT_EQ(cnt, 9);
    printf("-------------List all records with non-null 5 < id <= 8--------------\n");
    fileScan->OpenScan(*handler, 1, IM::GT, "5");
    cnt = 0;
    while (!fileScan->GetNextRec(*handler, nextRec)) {
        handler->recordHandler->PrintRecord(nextRec);
        cnt++;
    }
    ASSERT_EQ(cnt, 3);
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
        handler->recordHandler->PrintRecord(result[i]);
    }
    ASSERT_EQ(result.size(), 15);
    ASSERT_EQ(result.size(), orig.size());
    rmg->closeFile(*handler);
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
