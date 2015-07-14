#include <cstdlib>
#include <cstdio>
#include <ctime>

#include <iostream>
#include <string>
using namespace std;

#include "CBaseMatcher.h"
#include "CRabinKarpMatcher.h"
#include "CSimpleMatcher.h"

int testMatcher(CBaseMatcher *matcher)
{
    size_t index_matched = -1;
    string string_matched;
    const int NUM_SMAPLES = 16;
    char *samples_to_match [NUM_SMAPLES] = {
        "HELL", " HELL", " HELL ",
        "What the HELL ", "Hello", "hello", "ello world",
        "血界战线", "=血界战线", "看血界战线!", "强烈推荐血界战线!!",
        "首先看一下来自Wolfram的定义马尔可夫链是随机变量{X_t}的集合（t贯穿0,1,...），给定当前的状态，未来与过去条件独立。Wikipedia的定义更清楚一点儿...马尔可夫链是具有马尔可夫性质的随机过程...[这意味着]状态改变是概率性的，未来的状态仅仅依赖当前的状态。马尔可夫链具有多种用途，现在让我看一下如何用它生产看起来像模像样的胡言乱语。算法如下,找一个作为语料库的文本，语料库用于选择接下来的转换。从文本中两个连续的单词开始，最后的两个单词构成当前状态。生成下一个单词的过程就是马尔可夫转换。为了生成下一个单词，首先查看语料库，查找这两个单词之后跟着的单词。从它们中随机选择一个。重复2，直到生成的文本达到需要的大小。",
        "简短地生成随机密码,包括大小写字母、数字,可以指定密码长度'''#生成随机",
        "Python面试题笔试题:标准库random实现了一个随机数生成器,实例代码如下",
        "上一篇:使用Python批量替换指定目录所有文件中的指定文本 下一篇",
        "文本文档是以TXT后缀名的文件，在windows页面重点及鼠标右键-〉新建-〉在下拉子菜单中有文本文档一项。 详细：txt文件是微软在操作系统上附带的一种文本格式，是最常见的一种文件格式，早在DOS时代应用就很多，主要存文本信息，即为文字信.",
    };
    for (int i = 0; i!= NUM_SMAPLES; ++i)
    {
        matcher->match(samples_to_match[i],
            &string_matched, &index_matched
        );
        // fprintf(stdout,
        //     "`%s`:\nPattern [%s] occurs in offset [%lu]\n",
        //     samples_to_match[i],
        //     string_matched.c_str(), index_matched
        // );
    }

    return 0;
}

int main(int argc, char const *argv[])
{
    const char *keywords [6] = {
        "hello", "world", 
        "HELL", "WORLD",
        "血界战线", "吹响!上悠风号",
    };

    clock_t start, end;
    const int TEST_ROUND = 100000;
    CBaseMatcher *matcher = NULL;

    cout << "testing CRabinKarpMatcher..." << endl;
    start = clock();
    matcher = new CRabinKarpMatcher(keywords, 6);
    for (int i=0; i!=TEST_ROUND; ++i)
        testMatcher(matcher);
    delete matcher;
    end = clock();
    cout << end-start << endl;

    cout << endl;

    cout << "testing CSimpleMatcher..." << endl;
    start = clock();
    matcher = new CSimpleMatcher(keywords, 6);
    for (int i=0; i!=TEST_ROUND; ++i)
        testMatcher(matcher);
    delete matcher;
    end = clock();
    cout << end-start << endl;

    return 0;
}
