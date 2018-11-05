#include "../filesystem/bufmanager/BufPageManager.h"
#include "../filesystem/fileio/FileManager.h"
#include <string>
#include <iostream>

using namespace std;

#ifndef RECORD_MANAGER 

class RecordManager{
private:
    char *dataPath;
    FileManager *fileManager;
    int fileID;
public:
    RecordManager();
    bool createFile(const char* name);
    bool openFile(const char* name);
};

#endif // CREATE