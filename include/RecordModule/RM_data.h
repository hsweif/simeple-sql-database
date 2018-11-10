#ifndef RM_DATA
#define RM_DATA

#include "../utils/pagedef.h"
#include "../utils/MyLinkList.h"
#include <vector>
#include <string>
#include <iostream>
#include <cstdio>

/**
 * Type define
 */
#define INT_TYPE 0
#define STR_TYPE 1
#define FLOAT_TYPE 2
#define DESCRIPTION 3

#define ITEM_LENGTH 16 
#define DESCRIPT_LENGTH 256

using namespace std;
struct RM_node
{
    int length;
    BufType ctx; //Context of the node
    RM_node(){}
    RM_node(BufType buf, int l);
    void setCtx(int n); 
    void setCtx(float f); 
    void setCtx(string s);
};

class RM_data
{
  private:
    int length;
    int strLength;
    int descriptLength;
    int recordLength;
    vector<string> dataTitle;
    vector<int> type;

  public:
    RM_data(vector<string> title, vector<int> type, int str_l =16, int dscp_l = 256);
    int dataLenth();
    int getSerializeRecord(BufType *rec, vector<RM_node> data, int &recordSize);
    vector<string> title();
    int getRecord(vector<RM_node> &result, BufType serializedBuf, int length);
};

#endif // !RM_data