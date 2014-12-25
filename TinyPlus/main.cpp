#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "tinyp_structs.h"
#include "tinyp_lexer.h"
#include "tinyp_parser.h"
#include "symboltable.h"
#include "syntax_tree.h"
#include "tinyp_codegen.h"

#include <vector>
using std::vector;

int main(int argc, char const *argv[])
{
    struct lex_state_t lex_state;
    vector<struct token_pair_t> token_pairs;

    syntax_tree_node_t *tree;
    parse_state_t parse_state;

/*
    const char* sample[]={
        "sample/example.tinyp",
        "sample/myexample.tinyp",
        "sample/experiment1_test1_in.txt",
        "sample/experiment1_test2_in.txt",
    };*/
    const char* sample[]={
        "sample/example2.tinyp",
        "sample/example.tinyp",
        "sample/experiment1_test2_in.txt",
        "sample/experiment2_test1_in.txt",
        "sample/experiment2_test2_in.txt",
    };
    for (int i=0; i != 1; ++i){
        printf("Lexical analyze file %s\n", sample[i]);
        init_lexer(sample[i], &lex_state, &token_pairs);

        parse(&lex_state, &token_pairs);
        printf("=======\n");
        printf("Tokens:\n");
        for (int i=0; i != token_pairs.size(); ++i){
            token_pair_print(&token_pairs[i]);
        }
        printf("=======\n\n");

        printf("Parsing Tokens...\n");
        init_parser(&parse_state, &token_pairs);
        tree = parse_program(&parse_state);
        printTree(tree);

        printf("Generating code...\n");
        struct code_gen_state_t code_gen_state;
        struct middle_code_t mid_code;
        vector<struct middle_code_t> v;
        init_code_generator(&mid_code, &code_gen_state);
        generate_code(tree, &mid_code, &code_gen_state, &v);
        for (int i=0; i!= v.size(); ++i)
            code_print(&v[i]);

        destroyTree(tree);
        destroy_lexer(&lex_state, &token_pairs);
    }
    
    return 0;
}
