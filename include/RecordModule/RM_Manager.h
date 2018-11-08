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

using namespace std;
class RM_Manager{
private:
    char *dataPath;
    FileManager *fileManager;
    BufPageManager *bufPageManager;
    int fileID;
    static int RID;
public:
    RM_Manager();
    int createFile(const char* name, int recordSize);
    bool openFile(const char* name, RM_FileHandle &fileHandle);
    int closeFile();
    int deleteFile(const char* name);
};

#endif // RM_MANAGER