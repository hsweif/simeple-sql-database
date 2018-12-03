#include<iostream>
#include<string>
#include<string.h>
#include "CommandModule/dataBaseManager.h"
#include "RecordModule/RM_Manager.h"
#include "RecordModule/RM_FileHandle.h"
#include "RecordModule/RM_FileScan.h"
#include "IndexModule/IndexHandle.h"
#include "utils/MyBitMap.h"
#include "IndexModule/bpt.h"
using namespace std;
int current = 0;
int tt = 0;
unsigned char h[61];
int main(int argc, char* argv[]) {
	if (argc != 3) {
		cout << "input error" << endl;
		return 0;
	}
	string type = argv[1];
	string dbName = argv[2];
	DIR *dir = NULL;
	if (type == "DATABASE")
		dir = UseDB(argv[2]);
	else //if(argc[1] == "TABLE")
	{
		cout << "input error" << endl;
		return 0;
	}
	if (dir == NULL)
	{
		cout << "useDB " << dbName << " error" << endl;
		return 0;
	}
	cout << "useDB " << dbName << " success" << endl;
	RM_Manager *rmg = new RM_Manager(argv[2]);
	string command;
	string commandWord[3];
	getline(cin, command);
	char *p;
	while (command != "exit") {
		char *c = (char*)command.c_str();
		p = strtok(c, " ");
		if(!p){
			printf("input command error\n");
			getline(cin, command);
			continue;
		}
		commandWord[0] = p;
		p = strtok(NULL, " ");
		if(!p){
			printf("input command error\n");
			getline(cin, command);
			continue;
		}		
		commandWord[1] = p;
		p = strtok(NULL, "");//all of last
		if(!p){
			printf("input command error\n");
			getline(cin, command);
			continue;
		}
		commandWord[2] = p;
		printf("%s\n", p);
		if(commandWord[0] == "create" && commandWord[1] == "TABLE"){
			rmg->createFile(commandWord[2].c_str(),10);
		}
		else if(commandWord[0] == "drop" && commandWord[1] == "TABLE"){
			rmg->deleteFile(commandWord[2].c_str());
		}		
		else if(commandWord[0] == "show" && commandWord[1] == "TABLE"){
			if(commandWord[2] != "ALL")
				rmg->showFile(commandWord[2].c_str());
			else
				rmg->showAllFile();
		}	
		getline(cin, command);
	}
	closeDB(dir);
	return 0;
}