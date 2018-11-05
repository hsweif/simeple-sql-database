#include "../include/RecordModule/RM_Manager.h"

RM_Manager::RM_Manager(){
    //从main执行路径算起
    this->dataPath = "./database/";
    this->fileManager = new FileManager();
    this->fileID = -1;
}

int RM_Manager::createFile(const char* name, int recordSize) {
    FileManager *fm = this->fileManager;
    char fileName[50];
    strcpy(fileName, this->dataPath);
    strcat(fileName, name);
    fm->createFile(fileName);
    int fileID;
    fm->openFile(fileName, fileID);
    PageHead *test = new PageHead(recordSize, PAGE_SIZE/recordSize, 0);
    fm->writePage(fileID, 0, test->encode2Buf(), 0);
    fm->closeFile(fileID);
}

bool RM_Manager::openFile(const char* name){
    char fileName[50];
    strcpy(fileName, this->dataPath);
    strcat(fileName, name);
    return this->fileManager->openFile(fileName, this->fileID);
}

int RM_Manager::closeFile(){
    return this->fileManager->closeFile(this->fileID);
}

int RM_Manager::deleteFile(const char* name) {
    // TODO: Delete file
    char fileName[50];
    strcpy(fileName, this->dataPath);
    strcat(fileName, name);
    return 0;
}

