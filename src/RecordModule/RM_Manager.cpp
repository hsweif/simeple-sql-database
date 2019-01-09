#include "RecordModule/RM_Manager.h"
#include <iostream>
using namespace std;
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

int RM_Manager::createFile(const char* name, int recordSize, int cNum) {
    FileManager *fm = this->fileManager;
    char fileName[50];
    strcpy(fileName, this->dataPath);
    strcat(fileName, name);
    printf("%s\n", fileName);
    fm->createFile(fileName);
    int fileID;
    fm->openFile(fileName, fileID);
    PageHead *test = new PageHead(recordSize, (int)(double(PAGE_INT_NUM - 1) / ((double)recordSize + 1 / 32)), 0, cNum);
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
    fileHandle.indexPath = idx;
    RM_FileHandle::CreateDir(idx);
    fileHandle.relatedRManager = this;
    fileHandle.init(this->fileID,this->bufPageManager);
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
    RM_FileHandle *handler = new RM_FileHandle();
    cout<<openFile(name,*handler)<<endl;
    printf("%s\n", name);
    handler->PrintColumnInfo();
    closeFile(*handler);
    //delete handler;
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
#ifdef __DARWIN_UNIX03
    struct stat st_buf;
    while (entry = readdir(dbDir))
    {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, ".." ) != 0) {
            int status = stat(entry->d_name, &st_buf);
            if (S_ISREG (st_buf.st_mode)) {
                showFile(entry->d_name);
                i++;
            }
        }
    }
#else
    while (entry = readdir(dbDir))
    {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0 && entry->d_type == 0) {
            showFile(entry->d_name);
            i++;
        }

    }
#endif
    closedir(dbDir);
}