#ifndef PAGE_HEAD
#define PAGE_HEAD
#include <cstring>
#include "../utils/pagedef.h"

class PageHead{
private: 
    BufType buf;
public:
    PageHead();
    PageHead(int rl, int rpp, int rsum);
    BufType encode2Buf();
};

#endif // PAGE_HEAD