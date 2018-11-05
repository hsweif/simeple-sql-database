#ifndef RC_DATA
#define RC_DATA

#include "../utils/pagedef.h"
#include <vector>

class RC_data{
private:
    int length;
    int itemLength;
    std::vector<char*> arr;
public:
    RC_data();
    RC_data(std::vector<char*> title, int itl);
    int dataLenth();
    int getSerializeRecord(std::vector<char*> data, BufType buf, int &recordSize);
    std::vector<char*> title();
    std::vector<char*> getRecord(BufType serializedBuf);
};

#endif // !RC_DATA