#include <RecordModule/RecordHandler.h>
#include "IndexModule/IndexHandle.h"

namespace IM{



IndexHandle::IndexHandle(int cNum)
{
    indexNum = 0;
    colNum = cNum;
    indexPath = "";
    for(int i = 0; i < colNum; i ++) {
        isIndex.push_back(false);
    }
}

int IndexHandle::SetIndexHandle(vector<string> tt, string idxPath)
{
    indexPath = idxPath;
    title = tt;
    colNum = title.size();
    return 0;
}

int IndexHandle::SetIndex(int pos, bool value)
{
    if(pos >= isIndex.size()) {
        return 1;
    }
    isIndex[pos] = value;
    return 0;
}

int IndexHandle::CreateIndex(char *indexName, int pos, bool forceEmpty)
{
    string indexStr(indexName);
    if(indexStr == "" || !isIndex[pos]) {
        return 1;
    }
    int cnt = 0;
    list<node>::iterator iter = index.begin();
    while(iter != index.end()) {
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
        cout << "Create Index: " << indexStr << endl;
        indexFile.open(indexFileName, ios::out);
        indexFile.close();
    }
    else{
        indexFile.close();
    }
    bpt::bplus_tree *bpTree = new bpt::bplus_tree((char*)indexFileName.data(), forceEmpty);
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

int IndexHandle::IndexAction(IM::IndexAction actionType, RM_Record &record, RM::RecordHandler *recordHandler)
{
    // TODO: Need to be checked
    list<node>::iterator iter = index.begin();
    for(int i = 0; i < colNum; i ++)
    {
        if(isIndex[i])
        {
            string ctx;
            RID rid;
            record.GetRid(rid);
            RM_node result;
            recordHandler->GetColumn(i, record, result);
            if(!result.isNull) {
                if(result.type == RM::INT) {
                    int n = result.num;
                    string tmp;
                    std::stringstream ss;
                    ss << n;
                    ss >> tmp;
                    ctx = tmp;
                }
                else if(result.type == RM::FLOAT) {
                    float f = result.fNum;
                    std::stringstream ss;
                    string tmp;
                    ss << f;
                    ss >> tmp;
                    ctx = tmp;
                }
                else {
                    ctx = result.str;
                }

                bpt::key_t kt(ctx.c_str());
                if(actionType == INSERT) {
                    iter->bpTree->insert(kt, rid);
                }
                else if(actionType == DELETE) {
                    iter->bpTree->remove(kt, rid);
                }
                else if(actionType == UPDATE) {
                    iter->bpTree->update(kt, rid);
                }
            }
            iter ++;
        }
    }
}

int IndexHandle::SearchRange(list<RID> &result, char *leftValue, char *rightValue, CompOp comOP, int col)
{
    // TODO: Undone.
    RID *searched = new RID[MAX_RESULT];
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
            if(comOP == IM::LS || comOP == IM::LEQ) {
                resultNum = indexTree->search_range(&left, right, searched, MAX_RESULT);
            }
            else if(comOP == IM::GT || comOP == IM::GEQ) {
                resultNum = indexTree->search_range(&right, left, searched, MAX_RESULT);
            }
            else if(comOP == IM::EQ) {
                resultNum = indexTree->search_range(&right, right, searched, MAX_RESULT);
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

bool IndexHandle::Existed(int pos, char *key)
{
    if(!isIndex[pos]) {
        cout << pos << "is not index" << endl;
        return false;
    }
    auto iter = index.begin();
    for(int i = 0; i < colNum; i ++)
    {
        if(i == pos)
        {
            bpt::key_t keyValue(key);
            bpt::bplus_tree *bpTree = iter->bpTree;
            RID *tmp = new RID;
            vector<RID> result;
            int ret = bpTree->search(keyValue, tmp);
            if(ret == 0) {
                return true;
            } else{
                return false;
            }
        }
        if(isIndex[i]) {
            iter ++;
        }
    }
    return false;
}

}
