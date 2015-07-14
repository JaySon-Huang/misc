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
        const string &str_to_match, unsigned long index_begin,
        const string &pattern)
    {
        for (unsigned int offset = 0; offset != pattern.size(); ++offset)
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
