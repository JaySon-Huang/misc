#ifndef __SIMPLE_MATCHER__
#define __SIMPLE_MATCHER__

#include <cstdlib>
#include <cstdio>
#include <cstring>

#include <vector>
#include <string>
using std::vector;
using std::string;

#include "CBaseMatcher.h"

class CSimpleMatcher : public CBaseMatcher
{
private:
    vector<string> m_words;
public:
    CSimpleMatcher(const char** keywords, const int keywords_cnt);
    ~CSimpleMatcher(){}

    virtual bool match(
        const string &str_to_match,
        string *string_match,
        size_t *index_match);
};

#endif
