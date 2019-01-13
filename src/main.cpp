#include "RecordModule/RM_Manager.h"
#include "RecordModule/RM_FileHandle.h"
#include "RecordModule/RecordHandler.h"
#include "RecordModule/RM_FileScan.h"
#include "RecordModule/RM_Record.h"
#include "IndexModule/IndexManager.h"
#include "utils/MyBitMap.h"
#include "IndexModule/bpt.h"
#include "CommandModule/dataBaseManager.h"
#include "parser.hpp"
#include <vector>
#include <string>

// 原先声明在头文件里的全局变量。。。
int current = 0;
int tt = 0;
unsigned char h[61];

using namespace std;

int main(int argc, char* argv[])
{
	MyBitMap::initConst();
	string DBPath = "../TestDataSet/";
	if(argc == 1)
	{
		printf("Usage:\n");
		printf("    ./main test1.sql test2.sql, ..., testn.sql (read from files)\n");
		printf("    ./main (for interaction)\n");
		printf("    Please put your test file under {REPO_DIR}/TestDataSet/\n");
		string command;
		printf("Please input your SQL command.\n");
		printf(">>>>> ");
        getline(cin,command);
        while(command != "exit")
        {
            // printf((currentDB+'>').c_str());
            ParseCommand(command);
            printf(">>>>> ");
			getline(cin,command);
        }
		return 1;
	}
	for(int i = 1;i <= argc - 1; i ++) {
		ParseInput(DBPath+argv[i]);
	}
	return 0;
}