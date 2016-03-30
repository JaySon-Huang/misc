#ifndef __TINYP_CODEGENERATOT_H__
#define __TINYP_CODEGENERATOT_H__

#include "tinyp_structs.h"

struct middle_code_t {
    string op;
    string larg, rarg;
    string store;
    int lineno;

    int islabel;
};

struct code_gen_state_t {
    int label_index;  // 存储下一个分配的label下标
    int tmp_index;    // 存储下一个临时变量下标
    int code_line;    // 存储代码行数
};

void init_code_generator(struct middle_code_t *pmid_code,
                         struct code_gen_state_t *pstate);

void generate_code(struct syntax_tree_node_t *root,
                   struct middle_code_t *pfa_code,
                   struct code_gen_state_t *pstate,
                   vector<struct middle_code_t> *pcode_vec);

void code_print(struct middle_code_t *pcode);

#endif
