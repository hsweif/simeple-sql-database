#ifndef DB_MANAGER
#define DB_MANAGER
#include <dirent.h>
#include <iostream>
#include <string>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

using namespace std;
int CreateDB(char* dbName){
	string str = dbName;
	str = "../database/"+str;
#ifdef WIN32
	if(_mkdir(str.c_str()) != 0){
		cout<<"createDB "<<dbName<<" error"<<endl;
		return -1;
	}
#endif
#ifdef __DARWIN_UNIX03
	mkdir(str.c_str(), S_IRWXU);
#endif
	cout<<"createDB "<<dbName<<" success!"<<endl;
	return 0;	
}

int DropDB(char* dbName){
	string str = dbName;
	str = "../database/"+str;
#ifdef WIN32
	if(_rmdir(str.c_str()) != 0){
		cout<<"dropDB "<<dbName<<" error"<<endl;
		return -1;
	}
#endif
#ifdef __DARWIN_UNIX03
	// FIXME: Dangerous?
	string command = "rm -r " + str;
	system(command.c_str());
#endif
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
	printf("DATABASE:%s\n",dbName);

#ifdef __DARWIN_UNIX03
    struct stat st_buf;
    while (entry = readdir(dir))
    {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, ".." ) != 0) {
        	string path = "../database/";
        	path += dbName;
			path += "/";
            int l = path.length() + strlen(entry->d_name);
            int cnt = 0;
            char filePath[l];
            memset(filePath, 0, sizeof(filePath));
            strcpy(filePath, path.c_str());
            strcat(filePath, entry->d_name);
            int status = stat(filePath, &st_buf);
            if (S_ISREG (st_buf.st_mode)) {
				printf("	tablename%d=%s\n",i,entry->d_name);
				i++;
			}
        }
    }
#else
    while(entry=readdir(dir))
    {
    	if(strcmp(entry->d_name,".") != 0 && strcmp(entry->d_name,"..") != 0 && entry->d_type == 0){
    		printf("	tablename%d=%s\n",i,entry->d_name);
	    	i++;
    	}
    }
#endif
    closeDB(dir);
    return 0;
}

int showAllDB(){
	DIR* dir = opendir("../database");
	if(dir == NULL)
	{
		cout<<"showAllDB error"<<endl;
		return -1;
	}	
	struct dirent *entry;
	int i = 0;
    while(entry=readdir(dir))
    {
    	if(strcmp(entry->d_name,".") != 0 && strcmp(entry->d_name,"..") != 0){
    		showDB(entry->d_name);
	    	i++;
    	}
    }
    closeDB(dir);
    return 0;
}
#endif