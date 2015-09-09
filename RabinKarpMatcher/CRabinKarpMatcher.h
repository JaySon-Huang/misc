#ifndef __RABIN_KARP_MATCHER__
#define __RABIN_KARP_MATCHER__

#include <cstdlib>
#include <cstdio>
#include <cstring>

#include <string>
#include <vector>
using std::vector;
using std::string;

#include "CBaseMatcher.h"

class CWordInformation
{
public:
    const static unsigned int HASH_D = 255;
    const static unsigned int HASH_MOD = 6999997;
public:
    string m_word;
    unsigned int m_hash;
    unsigned int m_top_muti;
public:
    CWordInformation(const string &word);

    static unsigned int hash_of(
        const string &word,
        size_t index_begin, size_t length);
};

class CRabinKarpMatcher : public CBaseMatcher
{
private:
    vector<CWordInformation> m_word_infos;
public:
    CRabinKarpMatcher(const char** keywords, const int keywords_cnt);
    ~CRabinKarpMatcher(){}

    virtual bool match(
        const string &str_to_match,
        string *string_match=NULL,
        size_t *index_match=NULL);
};

#endif  // end of __RABIN_KARP_MATCHER__
