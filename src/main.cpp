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
		ParseInput(DBPath+"default.sql");
		printf("Usage: ./main test.sql\n");
		printf("    Please put your test file under {REPO_DIR}/TestDataSet/\n");
		printf("    Running default.sql by default\n");
		return 1;
	}
	ParseInput(DBPath+argv[1]);
	return 0;
}