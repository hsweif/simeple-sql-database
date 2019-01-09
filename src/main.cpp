#include "RecordModule/RM_Manager.h"
#include "RecordModule/RM_FileHandle.h"
#include "RecordModule/RecordHandler.h"
#include "RecordModule/RM_FileScan.h"
#include "RecordModule/RM_Record.h"
#include "IndexModule/IndexHandle.h"
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
	if(argc != 2)
	{
		printf("Usage:\n");
		printf("    ./main test.sql (read from files)\n");
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
	ParseInput(DBPath+argv[1]);
	return 0;
}