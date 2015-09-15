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
    const static uint32_t HASH_D = 255;
    const static uint32_t HASH_MOD = 6999997;
public:
    string m_word;
    uint32_t m_hash;
    uint32_t m_top_muti;
public:
    CWordInformation(const string &word);

    static uint32_t hash_of(
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
