#ifndef __TINYP_CODEGENERATOT_H__
#define __TINYP_CODEGENERATOT_H__ 

#include "tinyp_structs.h"

struct middle_code_t {
    int LTrue,LFalse;
    int LBegin,LNext;
    int is_stmt;
};

struct code_gen_state_t{
    int label_index;
    int tmp_index;
    int code_line;
};

void init_code_generator(
    struct middle_code_t *pmid_code,
    struct code_gen_state_t *pstate);

char* 
generate_code(struct syntax_tree_node_t *root,
    struct middle_code_t* pmid_code,
    struct code_gen_state_t *pstate);

#endif
