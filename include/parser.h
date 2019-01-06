#ifndef PARSER_H
#define PARSER_H
#include "SQLParser.h"
#include "util/sqlhelper.h"
#include <iostream>
#include <cctype>
#include <algorithm>
#include <iterator>
#include <string>
#include<string.h>
#include "CommandModule/dataBaseManager.h"
#include "RecordModule/RM_Manager.h"
#include "RecordModule/RM_FileHandle.h"
#include "RecordModule/RM_FileScan.h"
#include "IndexModule/IndexHandle.h"
#include "utils/MyBitMap.h"
#include "IndexModule/bpt.h"
#include <fstream>
using namespace std;

RM_Manager *rmg;
string currentDB;
void toUpper(string &s){
	transform(s.begin(), s.end(), s.begin(), ::toupper);
}
int ParseDBCommand(string command){
	string commandWord[3];
	char *p;
	char *c = (char*)command.c_str();
	p = strtok(c, " ");
	if (!p) {
		printf("input command error\n");
		return -1;
	}
	commandWord[0] = p;
	p = strtok(NULL, " ");
	if (!p) {
		printf("input command error\n");
		return -1;
	}
	commandWord[1] = p;
	p = strtok(NULL, "");//all of last
	if (!p) {
		printf("input command error\n");
		return -1;
	}
	commandWord[2] = p;
	toUpper(commandWord[0]);
	toUpper(commandWord[1]);
	DIR *dir = NULL;
	if(commandWord[0] == "CREATE" && commandWord[1] == "DATABASE"){
		char *dbName = (char*)commandWord[2].c_str();
		CreateDB(dbName);
	}
	else if(commandWord[0] == "DROP" && commandWord[1] == "DATABASE"){
		char *dbName = (char*)commandWord[2].c_str();
		DropDB(dbName);		
	}
	else if(commandWord[0] == "USE" && commandWord[1] == "DATABASE"){
		char *dbName = (char*)commandWord[2].c_str();
		dir = UseDB(dbName);		
		if (dir == NULL)
		{
			cout << "useDB " << dbName << " error" << endl;
			return -1;
		}
		currentDB = (string)dbName;
		rmg = new RM_Manager(dbName);
	}
	else if(commandWord[0] == "SHOW" && commandWord[1] == "DATABASE"){
		char *dbName = (char*)commandWord[2].c_str();
		showDB(dbName);		
	}
	else if(commandWord[0] == "DROP" && commandWord[1] == "TABLE"){
		if(rmg == NULL)
			return -1;
		rmg->deleteFile(commandWord[2].c_str());
	}
	else if (commandWord[0] == "SHOW" && commandWord[1] == "TABLE") {
		if(rmg == NULL)
			return -1;		
		if (commandWord[2] != "ALL"){
			rmg->showFile(commandWord[2].c_str());
		}
		else
			rmg->showAllFile();
	}
	return 0;	
}
RM::ItemType transType(hsql::DataType type){
	if(type == hsql::DataType::INT)
		return RM::ItemType::INT;
	else if(type == hsql::DataType::FLOAT)
		return RM::ItemType::FLOAT;
	else if(type == hsql::DataType::CHAR)
		return RM::ItemType::CHAR;
	else return RM::ItemType::ERROR;
}
bool checkOp(hsql::OperatorType op){
	if(op == hsql::OperatorType::kOpNot
	||op == hsql::OperatorType::kOpIsNull
	||op == hsql::OperatorType::kOpEquals
	||op == hsql::OperatorType::kOpNotEquals
	||op == hsql::OperatorType::kOpLess
	||op == hsql::OperatorType::kOpGreater
	||op == hsql::OperatorType::kOpLessEq
	||op == hsql::OperatorType::kOpGreaterEq)
		return true;
	return false;
}
//(a<b)AND(b<2)AND(c<3) => push_back(a<b);...
int getExpr(hsql::Expr *expr,std::vector<hsql::Expr*>* whereExprs){
	if(expr->opType == hsql::OperatorType::kOpAnd){
		cout<<"left"<<expr->expr->opType<<"right"<<expr->expr2->opType<<endl;
		return getExpr(expr->expr,whereExprs)+getExpr(expr->expr2,whereExprs);
	}
	else if(checkOp(expr->opType)){//legal op
		cout<<"unit"<<expr->opType<<endl;
		whereExprs->push_back(expr);
		return 0;
	}
	return 1;//wrong op
}
int getExprAnswer(){//4+2,"asd"+"asdsa",x+2+4
}
int executeCommand(const hsql::SQLStatement* stmt){
	if(stmt->isType(hsql::kStmtCreate)){//create table
		printf("create\n");
		if(rmg == NULL){
			printf("current path is not DBPath\n");
			return -1;
		}
		vector<string> title;
		std::vector<hsql::ColumnDefinition*> col = ((hsql::CreateStatement*)stmt)->columns[0];	
		int colNum = col.size();
		RM_FileHandle *handler = new RM_FileHandle();
		handler->recordHandler = new RM::RecordHandler(colNum);
		for(int i = 0;i < colNum;i++){
			title.push_back((string)(col[i]->name));
			handler->recordHandler->SetItemAttribute(i,col[i]->type.length,transType(col[i]->type.data_type),col[i]->nullable);
		}	
		handler->SetTitle(title);	
		int size = handler->recordHandler->GetRecordSize();
		rmg->createFile(((hsql::CreateStatement*)stmt)->tableName,size,colNum);
		rmg->openFile(((hsql::CreateStatement*)stmt)->tableName,*handler);
		// InitIndex 要在openfile后面
		handler->InitIndex(true);
		std::vector<int> mainKeys;
		if(((hsql::CreateStatement*)stmt)->primaryKeys != nullptr){
			for(char *key:*(((hsql::CreateStatement*)stmt)->primaryKeys)){
				printf("primaryKey:%s\n", key);
				for(int i=0;i<colNum;i++)
					if(title[i].compare(key)){
						mainKeys.push_back(i);
						//handler->SetMainKey(i);
						break;						
					}
			}
			handler->SetMainKey(mainKeys);
		}
		//handler->PrintTitle();
		rmg->closeFile(*handler);
		delete handler;
	}
	else if(stmt->isType(hsql::kStmtInsert)){
		if(rmg == NULL){
			printf("current path is not DBPath\n");
			return -1;
		}
		vector<RM_node> items;
		std::vector<char*> col = ((hsql::InsertStatement*)stmt)->columns[0];
		std::vector<hsql::Expr*> val = ((hsql::InsertStatement*)stmt)->values[0];	

	}
	else if(stmt->isType(hsql::kStmtDelete)){
		if(rmg == NULL){
			printf("current path is not DBPath\n");
			return -1;
		}
		hsql::Expr *expr = ((hsql::DeleteStatement*)stmt)->expr;
		if(expr->type != hsql::kExprOperator){
			printf("wrong type\n");
		}
		std::vector<hsql::Expr*>* whereExprs = new std::vector<hsql::Expr*>();
		int ret = getExpr(expr,whereExprs);
		printf("ret:%d exps.size:%d\n",ret,whereExprs->size());
		int whereExprsize = whereExprs->size();
		for(int i = 0;i < whereExprsize;i++){
			cout<<(*whereExprs)[i]->opType<<endl;
		}
		delete whereExprs;
	}
	else if(stmt->isType(hsql::kStmtUpdate)){
		if(rmg == NULL){
			printf("current path is not DBPath\n");
			return -1;
		}	
		string tableName = (string)(((hsql::UpdateStatement*)stmt)->table->name);
		cout<<"tableName:"<<tableName<<endl;
		std::vector<hsql::UpdateClause*> updates = ((hsql::UpdateStatement*)stmt)->updates[0];
		//get whereclause
		hsql::Expr *expr = ((hsql::UpdateStatement*)stmt)->where;
		std::vector<hsql::Expr*>* whereExprs = new std::vector<hsql::Expr*>();
		int ret = getExpr(expr,whereExprs);
		printf("ret:%d exps.size:%d\n",ret,whereExprs->size());
		int whereExprsize = whereExprs->size();
		for(int i = 0;i < whereExprsize;i++){
			cout<<(*whereExprs)[i]->opType<<endl;
		}	
		//get setExpr
		for(hsql::UpdateClause* update:updates){
			printf("update:%s\n",update->column);
		}
	}
	else if(stmt->isType(hsql::kStmtSelect)){
		if(rmg == NULL){
			printf("current path is not DBPath\n");
			return -1;
		}	
		//get tables
		std::vector<hsql::TableRef*> list = ((hsql::SelectStatement*)stmt)->fromTable->list[0];		
		for(hsql::TableRef* table:list){
			printf("table:%s\n", table->name);
		}
		//get cols
		std::vector<hsql::Expr*> selectList = ((hsql::SelectStatement*)stmt)->selectList[0];	
		for(hsql::Expr* expr:selectList){
			if(expr->table != NULL){
				if(expr->type == hsql::ExprType::kExprStar){
					printf("col_name:%s.*\n", expr->table);
				}
				else printf("col_name:%s.%s\n", expr->table,expr->name);
			}
			else {
				if(expr->type == hsql::ExprType::kExprStar)
					printf("col_name:*\n");
				else
					printf("col_name:%s\n", expr->name);
			}
		}
		//get whereClause
		hsql::Expr *expr = ((hsql::SelectStatement*)stmt)->whereClause;
		std::vector<hsql::Expr*>* whereExprs = new std::vector<hsql::Expr*>();
		int ret = getExpr(expr,whereExprs);
		printf("ret:%d exps.size:%d\n",ret,whereExprs->size());
		int whereExprsize = whereExprs->size();
		for(int i = 0;i < whereExprsize;i++){
			cout<<(*whereExprs)[i]->opType<<endl;
		}
	}
	return 0;
}
void ParseInput(){
	string command;
	printf(">");
	ifstream fin("../testcase/test1.txt");
	getline(fin,command);
	while(command != "exit"){
		hsql::SQLParserResult result;
		hsql::SQLParser::parse(command, &result);		
		if(result.isValid()){
			printf("Parsed successfully!\n");
			printf("Number of statements: %lu\n", result.size());

			for (auto i = 0u; i < result.size(); ++i) {
				// Print a statement summary.
				//hsql::printStatementInfo(result.getStatement(i));
				executeCommand(result.getStatement(i));
			}			
		} else {
			int ret = ParseDBCommand(command);
			if(ret < 0){
				fprintf(stderr, "Given string is not a valid SQL query.\n");
				fprintf(stderr, "%s (L%d:%d)\n",
						result.errorMsg(),
						result.errorLine(),
						result.errorColumn());
			}
			//return;
		}
		printf((currentDB+'>').c_str());
		if(!getline(fin,command))
			break;
	}
}

#endif