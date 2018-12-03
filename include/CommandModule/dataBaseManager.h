#ifndef DB_MANAGER
#define DB_MANAGER
#include<dirent.h>
#include<iostream>
#include<string>
#include<string.h>
#include<stdio.h>
#include <sys/types.h>
using namespace std;
int CreateDB(char* dbName){
	string str = dbName;
	str = "../database/"+str;
	if(_mkdir(str.c_str()) != 0){
		cout<<"createDB "<<dbName<<" error"<<endl;
		return -1;
	}
	cout<<"createDB "<<dbName<<" success!"<<endl;
	return 0;	
}

int DropDB(char* dbName){
	string str = dbName;
	str = "../database/"+str;
	if(_rmdir(str.c_str()) != 0){
		cout<<"dropDB "<<dbName<<" error"<<endl;
		return -1;
	}
	cout<<"dropDB "<<dbName<<" success!"<<endl;
	return 0;	
}

DIR* UseDB(char* dbName){
	string str = dbName;
	str = "../database/"+str;
	return opendir(str.c_str());	
}

int closeDB(DIR* dir){
	closedir(dir);
	return 0;
}

int showDB(char* dbName){
	string str = dbName;
	DIR* dir = UseDB(dbName);
	if(dir == NULL)
	{
		cout<<"showDB "<<dbName<<" error"<<endl;
		return -1;
	}	
	struct dirent *entry;
	int i = 0;
    while(entry=readdir(dir))
    {
    	if(strcmp(entry->d_name,".") != 0 && strcmp(entry->d_name,"..") != 0){
    		printf("tablename%d=%s\n",i,entry->d_name);
	    	i++;
    	}

    }
    closeDB(dir);
    return 0;
}
#endif