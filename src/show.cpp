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
	if(type == "DATABASE")
		showDB(argv[2]);
	else //if(argc[1] == "TABLE")
	{
		cout<<"input error"<<endl;
		return 0;
	}	
	return 0;
}