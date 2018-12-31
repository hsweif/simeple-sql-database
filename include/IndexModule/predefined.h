#ifndef PREDEFINED_H
#define PREDEFINED_H


#include <string.h>
#include <string>
#include "../RecordModule/RID.h"

namespace bpt {

/* predefined B+ info */
#define BP_ORDER 20

/* key/value type */
typedef RID value_t;
struct key_t {
    char k[16];

    key_t(const char *str = "")
    {
        //bzero(k, sizeof(k));
        memset(k,'0',strlen(k));
        strcpy(k, str);
    }

    bool operator != (const key_t &b)
    {
        for(int i = 0; i < 16; i ++) {
            if(this->k[i] != b.k[i]) {
                return true;
            }
        }
        return false;
    }
};

inline int keycmp(const key_t &a, const key_t &b) {
    int a_l = strlen(a.k), b_l = strlen(b.k);
    int x = strlen(a.k) - strlen(b.k);
    return x == 0 ? strcmp(a.k, b.k) : x;
}

#define OPERATOR_KEYCMP(type) \
    bool operator< (const key_t &l, const type &r) {\
        return keycmp(l, r.key) < 0;\
    }\
    bool operator< (const type &l, const key_t &r) {\
        return keycmp(l.key, r) < 0;\
    }\
    bool operator== (const key_t &l, const type &r) {\
        return keycmp(l, r.key) == 0;\
    }\
    bool operator== (const type &l, const key_t &r) {\
        return keycmp(l.key, r) == 0;\
    }

}

#endif /* end of PREDEFINED_H */
