#include<iostream>
#include<string>
#include "CommandModule/dataBaseManager.h"
using namespace std;
int main(int argc, char* argv[]) {
	if (argc != 3) {
		cout << "input error" << endl;
		return 0;
	}
	string type = argv[1];
	if (type == "DATABASE") {
		if (strcmp(argv[2], "ALL") != 0)
			showDB(argv[2]);
		else {
			DIR *dbDir = opendir("../database");
			if (dbDir == NULL)
			{
				cout << "showAllDB " <<  " error" << endl;
				return -1;
			}
			struct dirent *entry;
			int i = 0;
			while (entry = readdir(dbDir))
			{
				if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
					printf("dBName%d=%s\n", i, entry->d_name);
					showDB(entry->d_name);
					i++;
				}

			}
			closeDB(dbDir);
		}
	}
	else //if(argc[1] == "TABLE")
	{
		cout << "input error" << endl;
		return 0;
	}
	return 0;
}