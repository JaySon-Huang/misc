#include "CSimpleMatcher.h"



CSimpleMatcher::CSimpleMatcher(
    const char** keywords, const int keywords_cnt)
{
    for (int i=0; i != keywords_cnt; ++i)
    {
        m_words.push_back(keywords[i]);
    }
}

bool CSimpleMatcher::match(
    const string &str_to_match,
    string *string_match,
    size_t *index_match)
{
    for (auto pWord = m_words.begin();
        pWord != m_words.end(); ++ pWord)
    {
        if (str_to_match.size() < pWord->size())
        {
            continue;  // 待匹配的短句比关键词还短, 肯定不能匹配
        }

        for (int index_begin = 0;
            index_begin != str_to_match.length() - pWord->size()+1;
            ++index_begin)
        {
            if (this->__extract_match(str_to_match, index_begin, *pWord))
            {
                *string_match = *pWord;
                *index_match = index_begin;
                return true;
            }else{
                continue;
            }
        }
    }
    return false;
}


