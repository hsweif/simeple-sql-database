#include "RecordModule/RM_Manager.h"

RM_Manager::RM_Manager(char *dbName) {
    //从cmake-build-debug算起
    dataPath = new char[50];
    char path[50] = "../database/";
    strcat(path, dbName);
    strcat(path, "/");
    strcpy(this->dataPath, path);
    printf("%s\n", this->dataPath);
    this->fileManager = new FileManager();
    this->bufPageManager = new BufPageManager(fileManager);
    this->fileID = -1;
}

int RM_Manager::createFile(const char* name, int recordSize) {
    printf("%s\n", name);
    FileManager *fm = this->fileManager;
    char fileName[50];
    strcpy(fileName, this->dataPath);
    strcat(fileName, name);
    printf("%s\n", fileName);
    fm->createFile(fileName);
    int fileID;
    fm->openFile(fileName, fileID);
    PageHead *test = new PageHead(recordSize, (int)(double(PAGE_INT_NUM - 1) / ((double)recordSize + 1 / 32)), 0);
    fm->writePage(fileID, 0, test->encode2Buf(), 0);
    fm->closeFile(fileID);
}

bool RM_Manager::openFile(const char* name, RM_FileHandle &fileHandle) {
    char fileName[50];
    strcpy(fileName, this->dataPath);
    strcat(fileName, name);
    // FIXME: index file 现在要手动创建
    char txt[7] = ".index";
    bool result = this->fileManager->openFile(fileName, this->fileID);
    strcat(fileName, txt);
    fileHandle.init(this->fileID, this->bufPageManager, fileName);
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

void RM_Manager::showFile(const char* name) {
    //TODO:show status
    printf("%s\n", name);
}

void RM_Manager::showAllFile() {
    DIR *dbDir = opendir(this->dataPath);
    if (dbDir == NULL)
    {
        cout << "showAllTABLE " <<  " error" << endl;
        return;
    }
    struct dirent *entry;
    int i = 0;
    while (entry = readdir(dbDir))
    {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            showFile(entry->d_name);
            i++;
        }

    }
    closedir(dbDir);
}