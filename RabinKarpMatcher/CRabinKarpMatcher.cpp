#include "CRabinKarpMatcher.h"

CWordInformation::CWordInformation(const string &word): m_hash(0), m_top_muti(1)
{
    unsigned char next_char = '\0';
    for (int i=0; i!=word.size(); ++i)
    {
        next_char = word[i];
        m_hash = (m_hash * HASH_D + next_char) % HASH_MOD;
        if (i != word.size()-1)
        {
            m_top_muti = (m_top_muti * HASH_D) % HASH_MOD;
        }
    }
    fprintf(stdout, "%s(%u), hash[%u], top[%u]\n",
        word.c_str(), word.size(), m_hash, m_top_muti);
}

unsigned int CWordInformation::hash_of(
    const string &word,
    size_t index_begin, size_t length)
{
    unsigned int hash = 0;
    unsigned char next_char = '\0';
    for (size_t i = 0; i != length; ++i)
    {
        next_char = word[i+index_begin];
        hash = (hash * HASH_D + next_char) % HASH_MOD;
    }
    return hash;
}

bool CRabinKarpMatcher::match(const string &str_to_match)
{
    for(auto iter = m_keys_infos.begin();
        iter != m_keys_infos.end();
        ++iter)
    {
        string key = iter->first;
        unsigned long key_length = key.size();
        CWordInformation &info = iter->second;

        fprintf(stdout, "trying to match `%s`[%u] in `%s`\n", 
            key.c_str(), info.m_hash, str_to_match.c_str());

        if (str_to_match.size() < key_length)
        {
            continue;  // 待匹配的短句比关键词还短, 肯定不能匹配
        }

        unsigned char head_char = '\0';
        unsigned char next_char = '\0';
        // 计算初始部分的hash值
        unsigned int hash_val = CWordInformation::hash_of(
            str_to_match,
            0, key_length
        );

        // Rolling hash 进行匹配
        for (unsigned long index_begin = 0;
            index_begin != str_to_match.size() - key_length+1;
            ++index_begin)
        {

            if (hash_val == info.m_hash)
            {
                // 确认是否完全匹配, 防止哈希碰撞
                if (this->__extract_match(str_to_match, index_begin, key))
                {
                    fprintf(stdout, "Pattern [%s] occurs in offset [%u]\n\n",
                        key.c_str(), index_begin);
                    return true;
                }
            }else if (index_begin < str_to_match.size() - key_length)
            {
                // 注意取unsigned char才能匹配utf-8的中文
                head_char = str_to_match[index_begin];
                next_char = str_to_match[index_begin + key_length];

                // 使用Rolling Hash更新hash值
                hash_val = (
                    (CWordInformation::HASH_D * (
                        hash_val
                        - head_char * info.m_top_muti % CWordInformation::HASH_MOD
                        + CWordInformation::HASH_MOD)
                    + next_char) % CWordInformation::HASH_MOD
                );
            }
        }// 这个关键词不匹配
    }// 所有关键词都不匹配
    return false;
}

bool CRabinKarpMatcher::__extract_match(
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
