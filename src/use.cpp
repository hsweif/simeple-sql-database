#include<iostream>
#include<string>
#include "CommandModule/dataBaseManager.h"
using namespace std;
int main(int argc, char* argv[]){
	if(argc != 3){
		cout<<"input error"<<endl;
		return 0;
	}
	string type = argv[1];
	string dbName = argv[2];
	DIR *dir = NULL;
	if(type == "DATABASE")
		dir = UseDB(argv[2]);
	else //if(argc[1] == "TABLE")
	{
		cout<<"input error"<<endl;
		return 0;
	}	
	if(dir == NULL)
	{
		cout<<"useDB "<<dbName<<" error"<<endl;
		return 0;
	}	
	cout<<"useDB "<<dbName<<" success"<<endl;
	closeDB(dir);
	return 0;
}