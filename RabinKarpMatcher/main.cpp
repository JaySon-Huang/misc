#include <cstdlib>
#include <cstdio>
#include <ctime>

#include <string>
using namespace std;

#include "CBaseMatcher.h"
#include "CRabinKarpMatcher.h"
#include "CSimpleMatcher.h"

#include "gtest/gtest.h"

const int TEST_ROUND = 10000;

struct STTestSentence
{
public:
    string m_sentence;
    bool m_isMatched;
    size_t m_posMatched;
    STTestSentence(
        const char *sentence, bool is_matched,
        size_t pos_matched
    ): m_sentence(sentence), m_isMatched(is_matched), m_posMatched(pos_matched)
    {}
};

class MatcherTest : public testing::TestWithParam<STTestSentence>
{
public:
    static vector<STTestSentence> sentences;

    static vector<STTestSentence>& SetUpTestSentences() {
        // 英文
        sentences.push_back(STTestSentence("HELL", true, 0));
        sentences.push_back(STTestSentence(" HELL", true, 1));
        sentences.push_back(STTestSentence(" HELL ", true, 1));
        sentences.push_back(STTestSentence("ello world", true, 5));
        sentences.push_back(STTestSentence("Hello World", false, 0));
        // 中文
        sentences.push_back(STTestSentence("血界战线", true, 0));
        sentences.push_back(STTestSentence(" 血界战线", true, 1));
        sentences.push_back(STTestSentence("看血界战线", true, 3));
        sentences.push_back(STTestSentence("看幻界战线", false, 0));
        sentences.push_back(STTestSentence(
            "简短地生成随机密码,包括大小写字母、数字,可以指定密码长度'''#生成随机",
            false, 0)
        );
        sentences.push_back(STTestSentence(
            "Python面试题笔试题:标准库random实现了一个随机数生成器,实例代码如下",
            true, 0)
        );
        sentences.push_back(STTestSentence(
            "上一篇:使用Python批量替换指定目录所有文件中的指定文本 下一篇",
            true, 16)
        );
        sentences.push_back(STTestSentence(
            "文本文档是以TXT后缀名的文件，在windows页面重点及鼠标右键-〉新建-〉在下拉子菜单中有文本文档一项。 详细：txt文件是微软在操作系统上附带的一种文本格式，是最常见的一种文件格式，早在DOS时代应用就很多，主要存文本信息，即为文字信.",
            false, 0)
        );
        sentences.push_back(STTestSentence(
            "电视动画《幻界战线》改编自由日本漫画家内藤泰弘原作的同名漫画。作品动画化决定的消息在2014年6月4日发售的漫画杂志《Jump SQ》2014年7月号以及漫画单行本第9集上正式宣布[1] 。电视动画于2015年4月4日开始播出，在中国大陆地区的译名为《幻界战线》（血界战线）。",
            true, 338)
        );
        return sentences;
    }
protected:
    static CBaseMatcher *rk_matcher;
    static CBaseMatcher *s_matcher;
public:
    static void SetUpTestCase() {
        const static unsigned int NUM_KEYWORD = 7;
        const static char *KEYWORDS [NUM_KEYWORD] = {
            "hello", "world", "Python",
            "HELL", "WORLD", "Django",
            "血界战线",
        };

        rk_matcher = new CRabinKarpMatcher(KEYWORDS, NUM_KEYWORD);
        s_matcher  = new CSimpleMatcher(KEYWORDS, NUM_KEYWORD);
    }

    static void TearDownTestCase() {
        delete rk_matcher;
        rk_matcher = NULL;
        delete s_matcher;
        s_matcher = NULL;
    }
};
// 静态成员声明
CBaseMatcher *MatcherTest::rk_matcher = NULL;
CBaseMatcher *MatcherTest::s_matcher = NULL;
vector<STTestSentence> MatcherTest::sentences;

TEST_P(MatcherTest, Simple)
{
    size_t index_matched = -1;
    string string_matched;

    STTestSentence sentence = GetParam();

    EXPECT_EQ(sentence.m_isMatched,
        s_matcher->match(sentence.m_sentence, &string_matched, &index_matched));
    if (sentence.m_isMatched)
    {
        EXPECT_EQ(sentence.m_posMatched, index_matched)
            << "matched string:`" << string_matched << "` @ " << index_matched;
    }
}

TEST_P(MatcherTest, RabinKarp)
{
    size_t index_matched = -1;
    string string_matched;

    STTestSentence sentence = GetParam();

    EXPECT_EQ(sentence.m_isMatched,
        rk_matcher->match(sentence.m_sentence, &string_matched, &index_matched));
    if (sentence.m_isMatched)
    {
        EXPECT_EQ(sentence.m_posMatched, index_matched)
            << "matched string:`" << string_matched << "` @ " << index_matched;
    }
}

TEST_P(MatcherTest, Simple_Repeat)
{
    size_t index_matched = -1;
    string string_matched;
    STTestSentence sentence = GetParam();

    for (int i=0; i!=TEST_ROUND; ++i){
        EXPECT_EQ(sentence.m_isMatched,
            s_matcher->match(sentence.m_sentence, &string_matched, &index_matched));
    }
}

TEST_P(MatcherTest, RabinKarp_Repeat)
{
    size_t index_matched = -1;
    string string_matched;
    STTestSentence sentence = GetParam();

    for (int i=0; i!=TEST_ROUND; ++i)
    {
        EXPECT_EQ(sentence.m_isMatched,
            rk_matcher->match(sentence.m_sentence, &string_matched, &index_matched));
    }
}

INSTANTIATE_TEST_CASE_P(
    No, MatcherTest,
    testing::ValuesIn(MatcherTest::sentences)
);

int main(int argc, char **argv)
{
    MatcherTest::SetUpTestSentences();

    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
