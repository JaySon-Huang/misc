#ifndef __BASE_MATCHER__
#define __BASE_MATCHER__

#include <cstdlib>
#include <cstdio>
#include <cstring>

#include <string>
using std::string;

class CBaseMatcher
{
public:
    CBaseMatcher(){}
    virtual ~CBaseMatcher(){};

    virtual bool match(
        const string &str_to_match,
        string *string_match,
        size_t *index_match) = 0;

protected:
    static bool __extract_match(
        const string &str_to_match, size_t index_begin,
        const string &pattern)
    {
/*
        // memcmp 替代逐个字节的比较
        return (
            0 == memcmp(
                str_to_match.data()+index_begin,
                pattern.data(), pattern.size()
            )
        );
*/

        // 逐个字节比较
        for (size_t offset = 0; offset != pattern.size(); ++offset)
        {
            if (str_to_match[index_begin + offset] != pattern[offset])
            {
                return false;
            }
        }
        return true;

    }
};

#endif
