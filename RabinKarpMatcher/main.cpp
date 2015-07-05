#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <string>
#include <map>
using namespace std;

#include "CRabinKarpMatcher.h"

int main(int argc, char const *argv[])
{
    CRabinKarpMatcher matcher;
    matcher.match("HELL");
    matcher.match(" HELL");
    matcher.match(" HELL ");
    matcher.match("What the HELL ");
    matcher.match("Hello");
    matcher.match("hello");
    matcher.match("ello world");
    matcher.match("血界战线");
    matcher.match("=血界战线");
    matcher.match("看血界战线!");
    matcher.match("强烈推荐血界战线!!");
    return 0;
}
