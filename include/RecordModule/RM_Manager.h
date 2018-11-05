#ifndef RM_MANAGER 
#define RM_MANAGER 

#include <cstring>
#include <string>
#include <string.h>
#include <iostream>
#include "../bufmanager/BufPageManager.h"
#include "../fileio/FileManager.h"
#include "RM_PageHead.h"

using namespace std;
class RM_Manager{
private:
    char *dataPath;
    FileManager *fileManager;
    int fileID;
    static int RID;
public:
    RM_Manager();
    int createFile(const char* name, int recordSize);
    bool openFile(const char* name);
    int closeFile();
    int deleteFile(const char* name);
};

#endif // RM_MANAGER