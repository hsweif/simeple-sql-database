#ifndef INDEXHANDLE_H
#define INDEXHANDLE_H

#include <list>
#include <iostream>
#include <string>
#include "../RecordModule/RM_Record.h"

#include "bpt.h"
#include "predefined.h"

using namespace std;

namespace IM{

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
public:
    IndexHandle();
    IndexHandle(vector<string> tt);
    int CreateIndex(char *indexName, int pos);
    int DeleteIndex(char *indexName, int pos);
    int InsertRecord(RM_Record &record);
};

}
#endif
