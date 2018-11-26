#include "IndexModule/IndexHandle.h"

namespace IM{

IndexHandle::IndexHandle()
{
    indexNum = 0;
    colNum = 0;
}

IndexHandle::IndexHandle(vector<string> tt)
{
    IndexHandle();
    title = tt;
    colNum = title.size();
    for(int i = 0; i < colNum; i ++) {
        isIndex.push_back(false);
    }
}

int IndexHandle::CreateIndex(char *indexName, int pos)
{
    string indexStr(indexName);
    int cnt = 0;
    list<node>::iterator iter = index.begin();
    while(iter != index.end()) {
        if(indexStr == iter->item) {
            return 1;
        }
        iter ++;
    }
    isIndex[pos] = true;
    bpt::bplus_tree *bpTree = new bpt::bplus_tree(indexName);
    index.push_back(node(indexStr, bpTree));
    return 0;
}

int IndexHandle::DeleteIndex(char *indexName, int pos)
{
    string indexStr(indexName);
    isIndex[pos] = false;
    list<node>::iterator iter = index.begin();
    while(iter != index.end())
    {
        if(indexStr == iter->item)
        {
            index.erase(iter);
            return 0;
        }
        iter ++;
    }
    return 1;
}

int IndexHandle::InsertRecord(RM_Record &record)
{
    // TODO
    list<node>::iterator iter = index.begin();
    for(int i = 0; i < colNum; i ++)
    {
        if(isIndex[i])
        {
            string ctx;
            RID rid;
            record.GetColumn(i, &ctx);
            record.GetRid(rid);
            cout << "insert record index: " << ctx << endl;
            bpt::key_t kt(ctx.c_str());
            iter->bpTree->insert(kt, rid);
            iter ++;
        }
    }
}

}
