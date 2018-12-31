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
int checkType(string type) {
	if (type == "int")
		return RM::INT;
	else if (type == "string")
		return RM::CHAR;
	else if (type == "float")
	    return RM::FLOAT;
	else
		return RM::ERROR;
}
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
		if (!p) {
			printf("input command error\n");
			getline(cin, command);
			continue;
		}
		commandWord[0] = p;
		p = strtok(NULL, " ");
		if (!p) {
			printf("input command error\n");
			getline(cin, command);
			continue;
		}
		commandWord[1] = p;
		p = strtok(NULL, "");//all of last
		if (!p) {
			printf("input command error\n");
			getline(cin, command);
			continue;
		}
		commandWord[2] = p;
		printf("%s\n", p);
		if (commandWord[0] == "create" && commandWord[1] == "TABLE") {
			vector<string> title;
			vector<int> varType;
			const char *sep = ", ";
			//string str = "asd(int a,string b,int c,bool d)";
			int pos1 = commandWord[2].find("(");
			int pos2 = commandWord[2].find(")");
			string tableName = commandWord[2].substr(0, pos1);
			//cout << tableName << endl;
			string str1 = commandWord[2].substr(pos1 + 1, pos2 - pos1 - 1);
			//cout << str1 << endl;
			char *p;
			char *s = (char *)str1.c_str();
			int cnt = 0;
			int recordSize = 0;
			p = strtok(s, sep);
			bool innerError = false;
			while (p) {
				//printf("%s\n", p);
				if (cnt % 2 == 0) {
					title.push_back((string)p);
				}
				else {
					int t = checkType((string)p);
					if (t == ERR_TYPE) {
						printf("type error:%s\n", p);
						innerError = true;
						break;
					}
					else if (t == RM::INT || t == RM::FLOAT)
						recordSize++;
					else if (t == STR_TYPE)
						recordSize += ITEM_LENGTH / 4;
					else if (t == DESCRIPTION)
						recordSize += DESCRIPT_LENGTH / 4;
					varType.push_back(t);
				}
				cnt++;
				p = strtok(NULL, sep);
			}
			if (innerError) {
				getline(cin, command);
				continue;
			}
			//analyseCommand(commandWord[2]);
			rmg->createFile(tableName.c_str(), recordSize);
			RM_FileHandle *handler = new RM_FileHandle();
			rmg->openFile(tableName.c_str(), *handler);
			handler->SetType(varType);
			handler->SetTitle(title);
			handler->PrintTitle();
			rmg->closeFile(*handler);
			delete handler;
		}
		else if (commandWord[0] == "drop" && commandWord[1] == "TABLE") {
			rmg->deleteFile(commandWord[2].c_str());
		}
		else if (commandWord[0] == "show" && commandWord[1] == "TABLE") {
			if (commandWord[2] != "ALL")
				rmg->showFile(commandWord[2].c_str());
			else
				rmg->showAllFile();
		}
		getline(cin, command);
	}
	closeDB(dir);
	return 0;
}

