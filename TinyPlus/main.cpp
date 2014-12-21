#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "tinyp_structs.h"
#include "tinyp_lexer.h"

#include <vector>
using std::vector;

int main(int argc, char const *argv[])
{
    struct lex_state_t lex_state;
    vector<struct token_pair_t> token_pairs;
    const char* sample[]={
        "sample/example.tinyp",
        "sample/myexample.tinyp",
        "sample/experiment1_test1_in.txt",
        "sample/experiment1_test2_in.txt",
    };
    for (int i=0; i != 4; ++i){
        printf("Lexical analyze file %s\n", sample[i]);
        init_lexer(sample[i], &lex_state, &token_pairs);

        parse(&lex_state, &token_pairs);
        printf("=======\n");
        printf("Tokens:\n");
        for (int i=0; i != token_pairs.size(); ++i){
            token_pair_print(&token_pairs[i]);
        }
        printf("=======\n\n");

        destroy_lexer(&lex_state, &token_pairs);
    }
    
    return 0;
}
