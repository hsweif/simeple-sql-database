#include "../include/RecordModule/RecordManager.h"

RecordManager::RecordManager(){
    //从main执行路径算起
    this->dataPath = "./database/";
    this->fileManager = new FileManager();
    this->fileID = -1;
}

bool RecordManager::createFile(const char* name) {
    char fileName[50];
    strcpy(fileName, this->dataPath);
    strcat(fileName, name);
    return this->fileManager->createFile(fileName);
}

bool RecordManager::openFile(const char* name){
    char fileName[50];
    strcpy(fileName, this->dataPath);
    strcat(fileName, name);
    return this->fileManager->openFile(fileName, this->fileID);
}

int RecordManager::closeFile(){
    return this->fileManager->closeFile(this->fileID);
}

int main(){
    RecordManager *rmg = new RecordManager();
    rmg->createFile("test");
    string test = rmg->openFile("test") ? "successfully opened" : "fail to open";
    cout << test << endl;
    return 0;
}