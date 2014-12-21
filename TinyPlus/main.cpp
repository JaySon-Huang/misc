#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "tinyp_structs.h"
#include "parse.h"

#include <vector>
using std::vector;

int main(int argc, char const *argv[])
{
    struct parse_state_t parse_state;
    vector<struct token_pair_t> token_pairs;
    const char* sample[]={
        "sample/example.tinyp",
        "sample/myexample.tinyp",
        "sample/experiment1_test1_in.txt",
        "sample/experiment1_test2_in.txt",
    };
    for (int i=0; i != 4; ++i){
        printf("Parsing file %s\n", sample[i]);
        init_parser(sample[i], &parse_state, &token_pairs);

        parse(&parse_state, &token_pairs);
        printf("=======\n");
        printf("Tokens:\n");
        for (int i=0; i != token_pairs.size(); ++i){
            token_pair_print(&token_pairs[i]);
            // printf("Node: (%d,%s)\n", token_pairs[i].kind, token_pairs[i].value.c_str());
        }
        printf("=======\n\n");

        destroy_parser(&parse_state, &token_pairs);
    }
    
    return 0;
}
