﻿#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "symboltable.h"
#include "syntax_tree.h"
#include "tinyp_codegen.h"
#include "tinyp_lexer.h"
#include "tinyp_parser.h"
#include "tinyp_structs.h"

#include <vector>
using std::vector;

int main(int argc, char const* argv[]) {
    int ret;
    struct lex_state_t lex_state;
    vector<token_pair_t> token_pairs;

    syntax_tree_node_t* tree;
    parse_state_t parse_state;

    const char* sample[] = {
        "sample/example2.tinyp",           "sample/example.tinyp",
        "sample/experiment1_test1_in.txt", "sample/experiment1_test2_in.txt",
        "sample/experiment2_test1_in.txt", "sample/experiment2_test2_in.txt",
    };
    for (int i = 0; i != 6; ++i) {
        printf("Lexical analyze file %s\n", sample[i]);
        init_lexer(sample[i], &lex_state, &token_pairs);

        ret = lexical(&lex_state, &token_pairs);
        if (ret == 1) {
            fprintf(stderr, "Error happned in lexical analyze.Exit.\n");
            exit(1);
        }
        printf("=======\n");
        printf("Tokens:\n");
        for (int i = 0; i != token_pairs.size(); ++i) {
            token_pair_print(&token_pairs[i]);
            if (i % 5 == 4)
                printf("\n");
            else
                printf("\t");
        }
        printf("\n=======\n\n");

        printf("Parsing Tokens...\n");
        init_parser(&parse_state, &token_pairs);
        tree = parse_program(&parse_state);
        // printSymbols();
        printf("Syntax Tree:\n");
        printTree(tree);

        printf("Generating code...\n");
        struct code_gen_state_t code_gen_state;
        struct middle_code_t mid_code;
        vector<struct middle_code_t> v;
        init_code_generator(&mid_code, &code_gen_state);
        generate_code(tree, &mid_code, &code_gen_state, &v);
        for (int i = 0; i != v.size(); ++i) code_print(&v[i]);

        destroyTree(tree);
        destroy_lexer(&lex_state, &token_pairs);
    }

    return 0;
}
