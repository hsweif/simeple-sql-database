#include "IndexModule/IndexHandle.h"

namespace IM{

IndexHandle::IndexHandle()
{
    indexNum = 0;
    colNum = 0;
    indexPath = "../database/index/";
}

IndexHandle::IndexHandle(vector<string> tt)
{
    IndexHandle();
    indexPath = "../database/index/";
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
        cout << iter->item << endl;
        if(indexStr == iter->item) {
            return 1;
        }
        iter ++;
    }
    isIndex[pos] = true;
    string indexFileName = indexPath + indexStr;
    fstream indexFile;
    indexFile.open(indexFileName, ios::in);
    if(!indexFile) {
        cout << "Index file doesn't exist" << endl;
    }

    cout << "index path: " << indexFileName << endl;
    bpt::bplus_tree *bpTree = new bpt::bplus_tree((char*)indexFileName.data());
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

int IndexHandle::SearchRange(vector<RID> &result, bpt::key_t &left, bpt::key_t &right, int comOP, int col)
{
    // TODO: Undone.
    // FIXME: Temporarily set to 0
    col = 0;
    RID rid;
    RID *searched;
    list<node>::iterator iter = index.begin();
    for(int i = 0; i < colNum; i ++)
    {
        if(i == col && isIndex[i])
        {
            if(comOP == SMALLER)
            {

            }
            break;
        }
        else if(isIndex[i])
        {
            /*
            string test = "dd";
            bpt::key_t kt((char*)test.data());
            if(iter->bpTree->search(kt, &rid) == -1) {
                cout << "fail to search: " << test << endl;
            } else{
                cout << "success to search: " << test << endl;
            }
            int x = 0, y = 0;
            rid.GetPageNum(x);
            rid.GetSlotNum(y);
            //TODO: 增加透过RID直接找到Record的功能
            cout << "result test: " << x << " " << y << endl;
             */
            iter ++;
        }
    }
}


}
