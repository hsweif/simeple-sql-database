#ifndef INDEXHANDLE_H
#define INDEXHANDLE_H

#include <list>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <errno.h>

#include "../RecordModule/RM_Record.h"
#include "../RecordModule/RecordHandler.h"

#include "bpt.h"
#include "predefined.h"

using namespace std;

namespace IM{
#define SMALLER 0
#define LARGER 1
#define MAX_RESULT 1000

enum IndexAction{
    UPDATE, DELETE, INSERT
};

enum CompOp{
    GT, LS, GEQ, LEQ, EQ, NEQ
};

struct node
{
    string item;
    bpt::bplus_tree *bpTree;
    node(string s, bpt::bplus_tree *bt)
    {
        item = s;
        bpTree = bt;
    }
};

class IndexHandle
{
private:
    list<node> index;
    vector<string> title;
    vector<bool> isIndex;
    int indexNum;
    int colNum;
    string indexPath;
public:
    IndexHandle();
    IndexHandle(vector<string> tt, string idxPath);
    int CreateIndex(char *indexName, int pos);
    int DeleteIndex(char *indexName, int pos);
    int IndexAction(IndexAction actionType, RM_Record &record, RM::RecordHandler *recordHandler);
    int SetIndex(int pos, bool value = true);
    int SearchRange(vector<RID> &result, char* left, char* right, CompOp comOP, int col);
    bool Existed(int pos, char *key);
};

}
#endif
