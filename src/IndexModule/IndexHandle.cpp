#include "IndexModule/IndexHandle.h"

namespace IM{

IndexHandle::IndexHandle()
{
    indexNum = 0;
    colNum = 0;
    indexPath = "../database/index/";
}

IndexHandle::IndexHandle(vector<string> tt, string idxPath)
{
    IndexHandle();
    indexPath = idxPath;
    title = tt;
    colNum = title.size();
    for(int i = 0; i < colNum; i ++) {
        isIndex.push_back(false);
    }
    // Temporarily
    // SetIndex(0, true);
}

int IndexHandle::SetIndex(int pos, bool value)
{
    if(pos >= isIndex.size()) {
        return 1;
    }
    isIndex[pos] = value;
    return 0;
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
        indexFile.open(indexFileName, ios::out);
        indexFile.close();
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

int IndexHandle::IndexAction(IM::IndexAction actionType, RM_Record &record)
{
    // TODO: Need to be checked
    list<node>::iterator iter = index.begin();
    for(int i = 0; i < colNum; i ++)
    {
        if(isIndex[i])
        {
            string ctx;
            RID rid;
            record.GetColumn(i, &ctx);
            record.GetRid(rid);
            bpt::key_t kt(ctx.c_str());
            if(actionType == INSERT) {
                iter->bpTree->insert(kt, rid);
            }
            else if(actionType == DELETE) {
                iter->bpTree->remove(kt);
            }
            else if(actionType == UPDATE) {
                iter->bpTree->update(kt, rid);
            }
            iter ++;
        }
    }
}

int IndexHandle::SearchRange(vector<RID> &result, char *leftValue, char *rightValue, int comOP, int col)
{
    // TODO: Undone.
    // FIXME: Temporarily set to 0
    RID *searched;
    result.clear();
    bpt::key_t left(leftValue);
    bpt::key_t right(rightValue);
    int resultNum = 0;
    auto iter = index.begin();
    for(int i = 0; i < colNum; i ++)
    {
        if(i == col && isIndex[i])
        {
            bpt::bplus_tree *indexTree = iter->bpTree;
            if(comOP == SMALLER) {
                resultNum = indexTree->search_range(&left, right, searched, MAX_RESULT);
            }
            else if(comOP == LARGER) {
                resultNum = indexTree->search_range(&right, left, searched, MAX_RESULT);
            }
            break;
        }
        else if(isIndex[i]) {
            iter ++;
        }
    }

    for(int i = 0; i < resultNum; i ++) {
        result.push_back(searched[i]);
    }

    return 0;
}


}
