#ifndef INDEXHANDLE_H
#define INDEXHANDLE_H

#include <list>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <errno.h>
#include <map>

#include "../RecordModule/RM_Record.h"
#include "../RecordModule/RecordHandler.h"

#include "bpt.h"
#include "predefined.h"
#include "config.h"

using namespace std;

namespace IM{
#define SMALLER 0
#define LARGER 1
#define MAX_RESULT 500000

enum IndexAction{
    UPDATE, DELETE, INSERT
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
    ~node()
    {
        delete bpTree;
    }
};

static class IndexManager;

class IndexHandle
{
private:
    list<node> index;
    vector<string> title;
    int indexNum;
    int colNum;
    string indexPath;
    vector<bool> isIndex;
    static map<string, IndexHandle*> indexHandlers;
public:
    IndexHandle(int cNum);
    int SetIndexHandle(vector<string> tt, string idxPath);
    int CreateIndex(char *indexName, int pos, bool forceEmpty);
    int DeleteIndex(char *indexName, int pos);
    int IndexAction(IndexAction actionType, RM_Record &record, RM::RecordHandler *recordHandler);
    int SetIndex(int pos, bool value = true);
    int SearchRange(list<RID> &result, char* left, char* right, CompOp comOP, int col);
    bool IsIndex(int pos) {return isIndex[pos];}
    bool Existed(int pos, char *key);
};

}
#endif
