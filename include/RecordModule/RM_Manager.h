#ifndef RM_MANAGER 
#define RM_MANAGER 

/* Local test
 *
#include "bufmanager/BufPageManager.h"
#include "fileio/FileManager.h"
#include "RecordModule/PageHead.h"
#include "RecordModule/RM_FileHandle.h"
*/
#include <cstring>
#include <string>
#include <string.h>
#include <iostream>
#include "../bufmanager/BufPageManager.h"
#include "../fileio/FileManager.h"
#include "RM_FileHandle.h"
#include "RM_PageHead.h"
#include <sys/stat.h>

using namespace std;
class RM_Manager{
private:
    char *dataPath;
    FileManager *fileManager;
    BufPageManager *bufPageManager;
    int fileID;
    static int RID;
public:
    RM_Manager(char *dbName);
    int createFile(const char* name, int recordSize, int cNum, RM_FileHandle &fileHandle);
    bool openFile(const char* name, RM_FileHandle &fileHandle);
    int closeFile(RM_FileHandle &fileHandle);
    int deleteFile(const char* name);
    void showFile(const char* name);
    void showAllFile();
};

#endif // RM_MANAGER