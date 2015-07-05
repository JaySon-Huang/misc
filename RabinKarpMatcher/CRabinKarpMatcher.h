#ifndef __RABIN_KARP_MATCHER__
#define __RABIN_KARP_MATCHER__

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <string>
#include <map>
using std::map;
using std::string;
// using namespace std;

class CWordInformation
{
public:
    const static unsigned int HASH_D = 255;
    const static unsigned int HASH_MOD = 6999997;
public:
    unsigned int m_hash;
    unsigned int m_top_muti;
public:
    CWordInformation(const string &word);

    static unsigned int hash_of(
        const string &word,
        size_t index_begin, size_t length);
};

class CRabinKarpMatcher
{
private:
    map<string, CWordInformation> m_keys_infos;
public:
    CRabinKarpMatcher()
    {
        const char *keywords [6] = {
            "hello", "world", 
            "HELL", "WORLD",
            "血界战线", "吹响!上悠风号",
        };
        for (int i=0; i!=6; ++i)
        {
            CWordInformation info(keywords[i]);
            m_keys_infos.insert(
                map<string, CWordInformation>::value_type(keywords[i], info)
            );
        }
    }

    virtual ~CRabinKarpMatcher(){}

    bool match(const string &str_to_match);
private:
    static bool __extract_match(
        const string &str_to_match, unsigned long index_begin,
        const string &pattern);
};

#endif  // end of __RABIN_KARP_MATCHER__
