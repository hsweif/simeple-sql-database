#include "RecordModule/RM_Manager.h"

RM_Manager::RM_Manager(){
    //从cmake-build-debug算起
    this->dataPath = "../database/";
    this->fileManager = new FileManager();
    this->bufPageManager = new BufPageManager(fileManager);
    this->fileID = -1;
}

int RM_Manager::createFile(const char* name, int recordSize) {
    FileManager *fm = this->fileManager;
    char fileName[50];
    strcpy(fileName, this->dataPath);
    strcat(fileName, name);
    cout<<fileName<<endl;
    fm->createFile(fileName);
    int fileID;
    fm->openFile(fileName, fileID);
    PageHead *test = new PageHead(recordSize, (int)(double(PAGE_INT_NUM-1) / ((double)recordSize+1/32)), 0);
    fm->writePage(fileID, 0, test->encode2Buf(), 0);
    fm->closeFile(fileID);
}

bool RM_Manager::openFile(const char* name, RM_FileHandle &fileHandle) {
    char fileName[50];
    char dirSym[10] = "_index/";
    strcpy(fileName, this->dataPath);
    strcat(fileName, name);
    bool result = this->fileManager->openFile(fileName, this->fileID);
    strcat(fileName, dirSym);
    string idx(fileName);
    RM_FileHandle::CreateDir(idx);
    // FIXME: index file 现在要手动创建
    fileHandle.init(this->fileID,this->bufPageManager, fileName);
    return result;
}

int RM_Manager::closeFile(RM_FileHandle &fileHandle) {
	fileHandle.updateHead();
	this->bufPageManager->close();
    return this->fileManager->closeFile(this->fileID);
}

int RM_Manager::deleteFile(const char* name) {
    // TODO: Delete file
    char fileName[50];
    strcpy(fileName, this->dataPath);
    strcat(fileName, name);
    return 0;
}

