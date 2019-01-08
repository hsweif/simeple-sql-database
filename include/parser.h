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
/*update record 
lPos:int pos,
r1Pos/r2Pos:int pos,-1 intliteral,-2 
binaryOp:none if unary
*/
struct UpdateExprPos
{
	int lPos;
	int r1Pos;
	int r2Pos;
	hsql::Expr* r;
	hsql::OperatorType binaryOp;
	UpdateExprPos(int lPos,int r1Pos,int r2Pos,hsql::OperatorType binaryOp,hsql::Expr* r):
	lPos(lPos),r1Pos(r1Pos),r2Pos(r2Pos),binaryOp(binaryOp),r(r){
		printf("%d %d %d\n", lPos,r1Pos,r2Pos);
	};
};
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
	if(type == hsql::DataType::INT){
		printf("intType\n");
		return RM::ItemType::INT;
	}
	else if(type == hsql::DataType::FLOAT){
		printf("floatType\n");
		return RM::ItemType::FLOAT;
	}
	else if(type == hsql::DataType::CHAR){
		printf("charType\n");
		return RM::ItemType::CHAR;
	}
	else {
		printf("errorType\n");
		return RM::ItemType::ERROR;
	}
}
IM::CompOp transOp(hsql::OperatorType op){
	if(op == hsql::OperatorType::kOpEquals){
		return IM::CompOp::EQ;
	}
	else if(op == hsql::OperatorType::kOpNotEquals){
		return IM::CompOp::NEQ;
	}
	else if(op == hsql::OperatorType::kOpLess){
		return IM::CompOp::LS;
	}
	else if(op == hsql::OperatorType::kOpGreater){
		return IM::CompOp::GT;
	}
	else if(op == hsql::OperatorType::kOpGreaterEq){
		return IM::CompOp::GEQ;
	}
	else if(op == hsql::OperatorType::kOpLessEq){
		return IM::CompOp::LEQ;
	}
	return IM::CompOp::ERROR;
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
		printf("%s",expr->expr->name);
		cout<<"unit"<<expr->opType<<" "<<expr->expr2->strName<<endl;
		whereExprs->push_back(expr);
		return 0;
	}
	return 1;//wrong op
}
int getExprAnswer(){//4+2,"asd"+"asdsa",x+2+4
}
//x=y+z,illegal return ERROR
RM::ItemType checkExprLegal(RM_node *r1,RM_node *r2,RM::ItemType lType,hsql::OperatorType op){
	if(lType == RM::ItemType::CHAR){
		if(r1->type != RM::ItemType::CHAR 
		|| r2->type != RM::ItemType::CHAR 
		|| op != hsql::OperatorType::kOpPlus){
			return RM::ItemType::ERROR;
		}
		return RM::ItemType::CHAR;
	}
	else if(lType == RM::ItemType::INT){
		if(r1->type == RM::ItemType::CHAR || r2->type == RM::ItemType::CHAR){
			return RM::ItemType::ERROR;
		}
		return RM::ItemType::INT;
	}
	else if(lType == RM::ItemType::FLOAT){
		if(r1->type == RM::ItemType::CHAR || r2->type == RM::ItemType::CHAR){
			return RM::ItemType::ERROR;
		}
		return RM::ItemType::FLOAT;
	}
	return RM::ItemType::ERROR;
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
		RM::ItemType temp;
		int ret, l;
		for(int i = 0;i < colNum;i++){
			title.push_back((string)(col[i]->name));
			ret = handler->recordHandler->SetItemAttribute(i,col[i]->type.length,transType(col[i]->type.data_type),col[i]->nullable);
			l = col[i]->type.length;
			temp = transType(col[i]->type.data_type);
		}
		handler->SetTitle(title);	
		int size = handler->recordHandler->GetRecordSize();
		rmg->createFile(((hsql::CreateStatement*)stmt)->tableName,size,colNum);
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
		rmg->openFile(((hsql::CreateStatement*)stmt)->tableName,*handler);
		// InitIndex 要在openfile后面
		handler->InitIndex(true);


		//handler->PrintTitle();
		rmg->closeFile(*handler);
		//delete handler;
	}
	else if(stmt->isType(hsql::kStmtInsert)){
		if(rmg == NULL){
			printf("current path is not DBPath\n");
			return -1;
		}
		//rmg = new RM_Manager((char*)currentDB.c_str());
		RM_FileHandle *handler = new RM_FileHandle();
        rmg->openFile(((hsql::InsertStatement*)stmt)->tableName,*handler);
		//handler->InitIndex(false);
		//std::vector<char*> col = ((hsql::InsertStatement*)stmt)->columns[0];
		std::vector<hsql::InsertValue*> values = ((hsql::InsertStatement*)stmt)->values[0];
        vector<RM_node> items;
		for(hsql::InsertValue* val:values){
			items.clear();
			//std::vector<hsql::Expr*> colValues = val->values[0];
			cout<<"insert:"<<endl;
            //items.clear();	
			for(hsql::Expr* expr:val->values[0]){
				if(!expr->isLiteral()){
					printf("wrong type\n");
					rmg->closeFile(*handler);
					return -1;
				}
				if(expr->isType(hsql::ExprType::kExprLiteralFloat)){
					RM_node fnode((float)(expr->fval));
					cout<<"float:"<<(float)(expr->fval)<<endl;
					items.push_back(fnode);
				}
				else if(expr->isType(hsql::ExprType::kExprLiteralInt)){
					RM_node inode((int)(expr->ival));
					cout<<"int:"<<(int)(expr->ival)<<endl;
					items.push_back(inode);
				}
				else if(expr->isType(hsql::ExprType::kExprLiteralString)){
					string name = expr->name;
					RM_node snode(name);
					cout<<"string:"<<(string)(expr->name)<<endl;
					items.push_back(snode);
				}
				else{
					RM_node node;
					cout<<"null"<<endl;	
					items.push_back(node);			
				}
			}
            RM_Record record;
            printf("colNum:%d\n",items.size());
            if(handler->recordHandler->MakeRecord(record, items)) {
                cout << "Error to make record." << endl;
            }
            handler->recordHandler->PrintRecord(record);
            handler->InsertRec(record);	
            printf("insert success\n");	
		}
		rmg->closeFile(*handler);
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
		RM_FileScan* fileScan = new RM_FileScan;
		RM_FileHandle *handler = new RM_FileHandle();
		rmg->openFile(((hsql::DeleteStatement*)stmt)->tableName,*handler);
		for(hsql::Expr *expr:*whereExprs){
			int colPos;
			int ret = handler->GetAttrIndex(expr->expr->name,colPos);
			if(ret){
				printf("attr not exist\n");
				return -1;
			}
			printf("%s is col:%d\n", expr->expr->name,colPos);
			if(expr->opType == hsql::OperatorType::kOpNot){
				fileScan->OpenScan(*handler,colPos,false);
			}
			else if(expr->opType == hsql::OperatorType::kOpIsNull){
				fileScan->OpenScan(*handler,colPos,true);
			}
			else{
				printf("compare to %s\n", (char*)(expr->expr2->strName.c_str()));
				fileScan->OpenScan(*handler,colPos,transOp(expr->opType),(char*)(expr->expr2->strName.c_str()));
			}
		}
		RM_Record nextRec;
		while(!fileScan->GetNextRec(*handler, nextRec)){
			printf("delete:\n");
			RID delRid;
			if(nextRec.GetRid(delRid)){
				printf("get rid error when del\n");
			}
			//handler->recordHandler->PrintRecord(nextRec);
			handler->DeleteRec(delRid);
		}
		fileScan->CloseScan();
		rmg->closeFile(*handler);
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
		RM_FileScan* fileScan = new RM_FileScan;
		RM_FileHandle *handler = new RM_FileHandle();	
		rmg->openFile(((hsql::UpdateStatement*)stmt)->table->name,*handler);
/*		std::vector<int> updatePos;
		int upPos;
		for(hsql::UpdateClause* update:updates){
			printf("update:%s\n",update->column);
			int ret = handler->GetAttrIndex(update->column,upPos);
			if(ret){
				printf("attr not exist\n");
				return -1;
			}
			updatePos.push_back(upPos);			
		}*/	
		//openScan
		for(hsql::Expr *expr:*whereExprs){
			int colPos;
			int ret = handler->GetAttrIndex(expr->expr->name,colPos);
			if(ret){
				printf("attr not exist\n");
				return -1;
			}
			printf("%s is col:%d\n", expr->expr->name,colPos);
			if(expr->opType == hsql::OperatorType::kOpNot){
				fileScan->OpenScan(*handler,colPos,false);
			}
			else if(expr->opType == hsql::OperatorType::kOpIsNull){
				fileScan->OpenScan(*handler,colPos,true);
			}
			else{
				printf("compare to %s\n", (char*)(expr->expr2->strName.c_str()));
				fileScan->OpenScan(*handler,colPos,transOp(expr->opType),(char*)(expr->expr2->strName.c_str()));
			}
		}
		//get updateExprs
		std::vector<UpdateExprPos> updatePoss;
		int upPos;
		for(hsql::UpdateClause* update:updates){
			int lPos,r1Pos,r2Pos;
			hsql::OperatorType binaryOp;
			printf("update:%s\n",update->column);
			int ret = handler->GetAttrIndex(update->column,lPos);
			if(ret){
				printf("attr not exist\n");
				fileScan->CloseScan();
				rmg->closeFile(*handler);
				delete whereExprs;
				return -1;
			}
			if(update->value->isLiteral()){
				r1Pos = -1;
				r2Pos = -1;
				binaryOp = hsql::OperatorType::kOpNone;
			}
			else if(update->value->isType(hsql::ExprType::kExprColumnRef)){
				int ret = handler->GetAttrIndex(update->value->name,r1Pos);
				if(ret){
					printf("attr not exist\n");
					fileScan->CloseScan();
					rmg->closeFile(*handler);
					delete whereExprs;
					return -1;
				}
				binaryOp = hsql::OperatorType::kOpNone;
				r2Pos = -1;				
			}
			else if(update->value->isType(hsql::ExprType::kExprOperator)){
				if(update->value->expr == NULL || update->value->expr2 == NULL){
					printf("error update set expr\n");
					fileScan->CloseScan();
					rmg->closeFile(*handler);
					delete whereExprs;
					return -1;
				}
				if(update->value->expr->isLiteral()){
					r1Pos = -1;
				}
				else{
					int ret = handler->GetAttrIndex(update->value->expr->name,r1Pos);
					if(ret){
						printf("attr not exist\n");
						fileScan->CloseScan();
						rmg->closeFile(*handler);
						delete whereExprs;
						return -1;
					}					
				}
				if(update->value->expr2->isLiteral()){
					r2Pos = -1;
				}
				else{
					int ret = handler->GetAttrIndex(update->value->expr2->name,r2Pos);
					if(ret){
						printf("attr not exist\n");
						fileScan->CloseScan();
						rmg->closeFile(*handler);
						delete whereExprs;
						return -1;
					}					
				}
				binaryOp = update->value->opType;
			}
			UpdateExprPos exprPos(lPos,r1Pos,r2Pos,binaryOp,update->value);
			updatePoss.push_back(exprPos);
		}
		//do update
		RM_Record nextRec;
		std::vector<RM_Record> records;
		int updateSize = updatePoss.size();
		while(!fileScan->GetNextRec(*handler, nextRec)){
			printf("update:\n");
			for(UpdateExprPos upExpr:updatePoss){
				if(upExpr.binaryOp == hsql::OperatorType::kOpNone){
					if(upExpr.r->isType(hsql::ExprType::kExprColumnRef)){//x=z
						RM_node node;
						handler->recordHandler->GetColumn(upExpr.r1Pos,nextRec,node);
						handler->recordHandler->SetColumn(upExpr.lPos,nextRec,node);
					}
					else{//x=4
						if(upExpr.r->isType(hsql::ExprType::kExprLiteralString)){
							string name = upExpr.r->name;
							RM_node node(name);
							handler->recordHandler->SetColumn(upExpr.lPos,nextRec,node);
						}
						else if(upExpr.r->isType(hsql::ExprType::kExprLiteralInt)){
							RM_node node((int)(upExpr.r->ival));
							handler->recordHandler->SetColumn(upExpr.lPos,nextRec,node);
						}
						else if(upExpr.r->isType(hsql::ExprType::kExprLiteralFloat)){
							RM_node node((float)(upExpr.r->fval));
							handler->recordHandler->SetColumn(upExpr.lPos,nextRec,node);
						}	
						else if(upExpr.r->isType(hsql::ExprType::kExprLiteralNull)){
							RM_node node;
							handler->recordHandler->SetColumn(upExpr.lPos,nextRec,node);
						}
						else{
							printf("update setExpr wrong\n");
							fileScan->CloseScan();
							rmg->closeFile(*handler);
							delete whereExprs;		
							return -1;					
						}
					}
				}
				else{
					RM_node *r1;
					RM_node *r2;
					if(upExpr.r1Pos < 0){//x=4+z or x=4+3
						switch(upExpr.r->expr->type){
							case hsql::ExprType::kExprLiteralFloat:
								r1 = new RM_node((float)(upExpr.r->expr->fval));
								break;
							case hsql::ExprType::kExprLiteralString:
								r1 = new RM_node((string)(upExpr.r->expr->name));
								break;
							case hsql::ExprType::kExprLiteralInt:
								r1 = new RM_node((int)(upExpr.r->expr->ival));
								break;
							default:
								printf("update setExpr wrong\n");
								fileScan->CloseScan();
								rmg->closeFile(*handler);
								delete whereExprs;		
								return -1;														 
						}
					}
					else{//x=z+4 or x=z+y
						r1 = new RM_node();
						handler->recordHandler->GetColumn(upExpr.r1Pos,nextRec,*r1);
					}
					if(upExpr.r2Pos < 0){//x=z+4 or x=4+3
						switch(upExpr.r->expr2->type){
							case hsql::ExprType::kExprLiteralFloat:
								r2 = new RM_node((float)(upExpr.r->expr2->fval));
								break;
							case hsql::ExprType::kExprLiteralString:
								r2 = new RM_node(upExpr.r->expr2->name);
								break;
							case hsql::ExprType::kExprLiteralInt:
								r2 = new RM_node((int)(upExpr.r->expr2->ival));
								break;
							default:
								printf("update setExpr wrong\n");
								fileScan->CloseScan();
								rmg->closeFile(*handler);
								delete whereExprs;
								delete r1;		
								return -1;														 
						}
					}
					else{//x=z+4 or x=z+y
						r2 = new RM_node();
						handler->recordHandler->GetColumn(upExpr.r2Pos,nextRec,*r2);
						cout<<"r2->type:"<<r2->type<<endl;
					}
					//if(checkExprLegal(r1,r2,))
					switch(upExpr.binaryOp){
						case hsql::OperatorType::kOpPlus:
							printf("%d+%f\n",r1->num,r2->fNum);
							break;
						case hsql::OperatorType::kOpMinus:
							printf("-\n");
							break;
						case hsql::OperatorType::kOpAsterisk:
							printf("*\n");
							break;
						case hsql::OperatorType::kOpSlash:
							printf("/\n");
							break;
						default:
							printf("update binaryOp wrong\n");
							fileScan->CloseScan();
							rmg->closeFile(*handler);
							delete whereExprs;
							delete r1;		
							delete r2;
							return -1;							
					}
					delete r1,r2;
				}
			}
			records.push_back(nextRec);	
			//handler->recordHandler->PrintRecord(nextRec);
			//handler->DeleteRec(delRid);
		}
		for(RM_Record rec:records){
			RID id;
			rec.GetRid(id);
			id.show();
			handler->UpdateRec(rec);			
		}
		fileScan->CloseScan();
		rmg->closeFile(*handler);
		delete whereExprs;
		//get setExpr
	}
	else if(stmt->isType(hsql::kStmtSelect)){
		if(rmg == NULL){
			printf("current path is not DBPath\n");
			return -1;
		}	
		printf("this is SelectCase\n");
		//get tables
		std::vector<string> tables;
		if(((hsql::SelectStatement*)stmt)->fromTable->list != NULL){
			std::vector<hsql::TableRef*> list = ((hsql::SelectStatement*)stmt)->fromTable->list[0];		
			for(hsql::TableRef* table:list){
				printf("table:%s\n", table->name);
				tables.push_back((string)(table->name));
			}
		}
		else
			tables.push_back(((hsql::SelectStatement*)stmt)->fromTable->name);
		//get cols
		std::vector<hsql::Expr*> selectList = ((hsql::SelectStatement*)stmt)->selectList[0];	
		bool selectAll = false;
		for(hsql::Expr* expr:selectList){
			if(expr->table != NULL){
				if(expr->type == hsql::ExprType::kExprStar){
					printf("col_name:%s.*\n", expr->table);
				}
				else printf("col_name:%s.%s\n", expr->table,expr->name);
			}
			else {
				if(expr->type == hsql::ExprType::kExprStar){
					selectAll = true;
					printf("col_name:*\n");
				}
				else
					printf("col_name:%s\n", expr->name);
			}
		}
		//get whereClause
		bool whereAll = false;
		hsql::Expr *expr = ((hsql::SelectStatement*)stmt)->whereClause;
		std::vector<hsql::Expr*>* whereExprs = new std::vector<hsql::Expr*>();
		if(expr == NULL)
			whereAll = true;
		else{
			int ret = getExpr(expr,whereExprs);
			printf("ret:%d exps.size:%d\n",ret,whereExprs->size());
			int whereExprsize = whereExprs->size();
			for(int i = 0;i < whereExprsize;i++){
				cout<<(*whereExprs)[i]->opType<<endl;
			}
		}
		if(whereAll){
			if(selectAll){
				printf("selectAll!!\n");
				RM_FileScan* fileScan = new RM_FileScan;
				RM_FileHandle *handler = new RM_FileHandle();
				rmg->openFile(tables[0].c_str(),*handler);
				fileScan->OpenScanAll(*handler);
				RM_Record mRec;
				while(fileScan->GetNextRec(*handler,mRec) != 1){
					handler->recordHandler->PrintRecord(mRec);
				}
				fileScan->CloseScan();
			}
			else{//not select all

			}
		}
		else{
			int whereSize = whereExprs->size();
			RM_FileScan* fileScan = new RM_FileScan;
			RM_FileHandle *handler = new RM_FileHandle();
			rmg->openFile(tables[0].c_str(),*handler);
			for(hsql::Expr *expr:*whereExprs){
				int colPos;
				int ret = handler->GetAttrIndex(expr->expr->name,colPos);
				if(ret){
					printf("attr not exist\n");
					return -1;
				}
				printf("%s is col:%d\n", expr->expr->name,colPos);
				if(expr->opType == hsql::OperatorType::kOpNot){
					fileScan->OpenScan(*handler,colPos,false);
				}
				else if(expr->opType == hsql::OperatorType::kOpIsNull){
					fileScan->OpenScan(*handler,colPos,true);
				}
				else{
					printf("compare to %s\n", (char*)(expr->expr2->strName.c_str()));
					fileScan->OpenScan(*handler,colPos,transOp(expr->opType),(char*)(expr->expr2->strName.c_str()));
				}
			}
			RM_Record nextRec;
			while(!fileScan->GetNextRec(*handler, nextRec)){
				handler->recordHandler->PrintRecord(nextRec);
			}
			fileScan->CloseScan();	
			rmg->closeFile(*handler);		
		}
		delete whereExprs;
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