#include <cstdlib>
#include <cstdio>
#include <string>
#include <ctime>
using std::string;

void RemoveAllSpace(string &str_in)
{
    size_t preFound = 0;
    size_t found = -1;
    string noNullStr;
    noNullStr.reserve(str_in.size());
    do {
        preFound = found+1;
        found = str_in.find(' ', found+1);
        noNullStr.append(str_in, preFound, found-preFound);
    } while (found != string::npos && found+1 < str_in.size());
    
    str_in.assign(noNullStr);
}

void RemoveAllSpaceOrigin(string &str_in)
{
    size_t found = 0;
    found = str_in.find(' ', found);
    while (found != string::npos) {
        str_in.erase(found, 1);
        found = str_in.find(' ', found);
    }
}

int main(int argc, char const *argv[])
{
    const static int ROUND = 100000000;
    clock_t begin, end;
    string str;
    
    begin = clock();
    for (int i=0; i!= ROUND; ++i){
        str = "Hello World ~";
        RemoveAllSpaceOrigin(str);
    }
    end = clock();
    printf("%.4f secs\n", ((double)(end-begin)/CLOCKS_PER_SEC));
    
    begin = clock();
    for (int i=0; i!= ROUND; ++i){
        str = "Hello World ~";
        RemoveAllSpace(str);
    }
    end = clock();
    printf("%.4f secs\n", ((double)(end-begin)/CLOCKS_PER_SEC));
    return 0;
}

