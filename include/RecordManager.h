#include "bufmanager/BufPageManager.h"
#include "fileio/FileManager.h"
#include <string>
#include <string.h>
#include <iostream>

using namespace std;

#ifndef RECORD_MANAGER 

/**
 * 宏定义第一个页文件各参数的offset
 */
#define LEN_POS 0
#define SUM_POS 4
#define EMPTY_POS 8

class RecordManager{
private:
    char *dataPath;
    FileManager *fileManager;
    int fileID;
    static int RID;
public:
    RecordManager();
    bool createFile(const char* name);
    bool openFile(const char* name);
    int closeFile();
};

#endif // CREATE