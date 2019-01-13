//
// Created by AlexFan on 2019/1/10.
//

#ifndef INDEXMANAGER_H
#define INDEXMANAGER_H

#include <map>
#include <iostream>
#include "IndexModule/IndexHandle.h"

using namespace std;
namespace IM{

class IndexManager{
public:
    static map<string, IM::IndexHandle*> indexHandlers;
    static int GetIndexHandler(string tableName, IM::IndexHandle *handler);
    static int SetIndexHandler(string tableName, IM::IndexHandle *handler);
};

}
#endif //INDEXMANAGER_H
