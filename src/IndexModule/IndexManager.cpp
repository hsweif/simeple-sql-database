//
// Created by AlexFan on 2019/1/10.
//

#include "IndexModule/IndexManager.h"

namespace IM{

map<string, IM::IndexHandle*> IM::IndexManager::indexHandlers;

int IndexManager::GetIndexHandler(string tableName, IM::IndexHandle *handler)
{
    auto iter = indexHandlers.find(tableName);
    if(iter == indexHandlers.end()) {
        return 1;
    }
    *handler = *indexHandlers[tableName];
    return 0;
}

int IndexManager::SetIndexHandler(string tableName, IM::IndexHandle *handler)
{
    auto iter = indexHandlers.find(tableName);
    if(iter == indexHandlers.end()) {
        indexHandlers.insert(pair<string, IndexHandle*>(tableName, handler));
    }
    else{
        *indexHandlers[tableName] = *handler;
    }
    return 0;
}


}
