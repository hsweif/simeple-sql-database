#ifndef RM_DATA
#define RM_DATA

#include "../utils/pagedef.h"
#include <vector>
#include <string>
#include <string.h>
#include <iostream>

using namespace std;
class RM_data{
private:
    int length;
    int itemLength;
    std::vector<string> arr;
public:
    RM_data();
    RM_data(std::vector<string> title, int itl);
    int dataLenth();
    BufType getSerializeRecord(std::vector<string> data, int &recordSize);
    std::vector<string> title();
    std::vector<string> getRecord(BufType serializedBuf, int length);
};

#endif // !RM_data