#include "../include/RecordModule/RM_data.h"

RM_node::RM_node(BufType buf, int l)
{
    length = l;
    ctx = buf;
}

void RM_node::setCtx(int n) 
{
    length = 1;
    ctx = new uint; 
    ctx[0] = (uint) n;
}

void RM_node::setCtx(float f) 
{
    length = 1;
    ctx = new uint; 
    ctx[0] = (uint) f;
}

void RM_node::setCtx(string s)
{
    int l = 0;
    int strLength = s.length();
    if(strLength < ITEM_LENGTH)
    {
        l = ITEM_LENGTH;
    }
    else if(strLength < DESCRIPT_LENGTH)
    {
        l = DESCRIPT_LENGTH;
    }
    else
    {
        cout << "[Error] item should not be more than 256 char" << endl;
        return;
    }
    length = l;
    ctx = new uint[l/4];
    for(int i = 0; i < l/4; i ++) {
        ctx[i] = 0;
    }
    int cnt = 0;
    int offset = 0;
    for(int i = 0; i < l; i ++)
    {
        uint c = 0;
        // cout << s[i] << " " << (uint)s[i] << " " << c << endl;
        if(i < strLength) {
            c = (uint)s[i] << offset;
        }
        // cout << c << "--";
        ctx[cnt] += c; 
        if(offset != 24)
        {
            offset += 8; 
        }
        else
        {
            // cout << "ctx: " << ctx[cnt] << endl;
            cnt ++;
            offset = 0;
        }
    }
}

RM_data::RM_data(vector<string> _title, vector<int> _type, int str_l, int dscp_l)
{
    dataTitle = _title;
    type = _type;
    length = dataTitle.size();
    strLength = str_l; // Should be 4*n
    descriptLength = dscp_l;
}

int RM_data::dataLenth()
{
    return length == 0 ? dataTitle.size() : length;
}

int RM_data::getSerializeRecord(BufType *rec, vector<RM_node> data, int &recordSize)
{
    if (data.size() != type.size())
    {
        return 1;
    }
    int size = 0;
    int data_l = data.size();
    for (int i = 0; i < data_l; i++)
    {
        if (type[i] == FLOAT_TYPE || type[i] == INT_TYPE)
        {
            size += 1;
            data[i].length = 1;
        }
        else if (type[i] == STR_TYPE)
        {
            int sLength = strLength / 4; 
            size += sLength;
            data[i].length = sLength;
        }
        else if (type[i] == DESCRIPTION)
        {
            int dLength = descriptLength / 4; 
            size += dLength;
            data[i].length = dLength;
        }
    }
    recordSize = size;
    recordLength = size;
    BufType buf = new uint[recordSize];
    int cnt = 0;
    for(int i = 0; i < data_l; i ++)
    {
        int node_l = data[i].length;
        for(int j = 0; j < node_l; j ++)
        {
            buf[cnt] = (uint)data[i].ctx[j]; 
            cnt ++;
        }
    }
    *rec = buf;
    return 0;
}

vector<string> RM_data::title()
{
    return dataTitle;
}

int RM_data::getRecord(vector<RM_node> &result, BufType serializedBuf, int bufLength)
{
    if(bufLength != recordLength)
    {
        cout << "[Error]Serialized record's length mismatch" << endl;
        cout << bufLength << " " << length << endl;
        return 1;
    }
    vector<RM_node> vec;
    int cnt = 0;
    int l = type.size();
    for (int i = 0; i < l; i++)
    {
        if(type[i] == INT_TYPE || type[i] == FLOAT_TYPE)
        {
            BufType number = new uint[1]; 
            number[0] = serializedBuf[cnt]; 
            vec.push_back(RM_node(number, 1));
            cnt ++;
        }
        else
        {
            int strLength = (type[i] == STR_TYPE) ? ITEM_LENGTH : DESCRIPT_LENGTH;
            int offset = 0;
            uint mask = 255;
            char c[strLength];
            for (int j = 0; j < strLength; j++)
            {
                uint tmp = (serializedBuf[cnt] & (mask << offset)) >> offset;
                c[j] = (char)tmp;
                // cout << "," << (char)tmp;
                if (offset == 24)
                {
                    cnt++;
                    offset = 0;
                }
                else
                {
                    offset += 8;
                }
            }
            string str(c);
            RM_node node;
            node.setCtx(str);
            vec.push_back(node);
        }
    }
    result = vec;
    return 0;
}