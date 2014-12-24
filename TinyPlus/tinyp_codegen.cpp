#include "assert.h"
#include "tinyp_codegen.h"

#include <vector>
using std::vector;

int code_gen_next_index(struct code_gen_state_t *pstate);

static void gen_stmt(
    struct syntax_tree_node_t *root,
    struct middle_code_t* pfa_code,
    struct code_gen_state_t *pstate,
    vector<struct middle_code_t> *pcode_vec);
static void gen_exp(
    struct syntax_tree_node_t *root,
    struct middle_code_t* pfa_code,
    struct code_gen_state_t *pstate,
    vector<struct middle_code_t> *pcode_vec);

void init_code_generator(
    struct middle_code_t *pmid_code,
    struct code_gen_state_t *pstate)
{
    pstate->code_line = 1;
    pstate->tmp_index = 0;
    pstate->label_index = 0;

    pmid_code->LBegin = pmid_code->LNext = pmid_code->LTrue = pmid_code->LFalse = 0;
}

int code_gen_next_index(struct code_gen_state_t *pstate)
{
    return (pstate->label_index)++;
}

int code_gen_next_tmp(struct code_gen_state_t *pstate)
{
    return (pstate->tmp_index)++;
}
void code_init_tmp(struct code_gen_state_t *pstate)
{
    pstate->tmp_index = 0;
}

void 
generate_code(
    struct syntax_tree_node_t *root,
    struct middle_code_t* pfa_code,
    struct code_gen_state_t *pstate,
    vector<struct middle_code_t> *pcode_vec)
{
    while (root != NULL){
        switch (root->node_type)
        {
        case STMT_IF:
        case STMT_REPEAT:
        case STMT_ASSIGN:
        case STMT_READ:
        case STMT_WRITE:
        case STMT_WHILE:
            gen_stmt(root, pfa_code, pstate, pcode_vec);
            break;
        case EXP_OP:
        case EXP_CONST:
        case EXP_ID:
        case EXP_STR:
            gen_exp(root, pfa_code, pstate, pcode_vec);
            break;
        default:
            break;
        }
        root = root->sibiling;
    }
}

static void gen_stmt(
    struct syntax_tree_node_t *root,
    struct middle_code_t* pfa_code,
    struct code_gen_state_t *pstate,
    vector<struct middle_code_t> *pcode_vec)
{
    char t[20];
    memset(t, 0, sizeof(t));
    struct middle_code_t code, label, label2;
    label.is_label = true;
    switch(root->node_type)
    {
    case STMT_IF:
        if (DEBUG)
            printf("Generating code for if.\n");
        code.op = "jnz";
        sprintf(t, "Label %2d", code_gen_next_index(pstate));
        code.store.assign(t);
        gen_exp(root->child[0], &code, pstate, pcode_vec);
        pcode_vec->push_back(code);
        if (DEBUG)
            printf("(%3s, %s, %s, %s)\n",
                code.op.c_str(), code.larg.c_str(), code.rarg.c_str(),
                code.store.c_str());
        generate_code(root->child[1], NULL, pstate, pcode_vec);
        
        // label记着if失败的位置
        label.store.assign(code.store);

        if (root->child[2] == NULL){
            printf("%s\n", label.store.c_str());
        }else{
            // 执行完if为真的情况,跳出if-else
            code.op = "jump";
            code.larg = "";code.rarg = "";
            sprintf(t, "Label %2d", code_gen_next_index(pstate));
            code.store.assign(t);
            pcode_vec->push_back(code);
            if (DEBUG)
                printf("(%3s, %s, %s, %s)\n",
                    code.op.c_str(), code.larg.c_str(), code.rarg.c_str(),
                    code.store.c_str());
            // 处理else
            printf("%s\n", label.store.c_str());
            pcode_vec->push_back(label);
            generate_code(root->child[2], NULL, pstate, pcode_vec);
            label.store.assign(code.store);
            // 完全跳出的位置
            pcode_vec->push_back(label);
            printf("%s\n", label.store.c_str());
        }
        break;

    case STMT_WHILE:
        if (DEBUG)
            printf("Generating code for while.\n");
        break;

    case STMT_REPEAT:
        if (DEBUG)
            printf("Generating code for repeat.\n");
        break;

    case STMT_ASSIGN:
        if (DEBUG)
            printf("Generating code for assign.\n");
        code.op = ":=";
        code.store = root->token.value;
        gen_exp(root->child[0], &code, pstate, pcode_vec);
        if (DEBUG)
            printf("(%3s, %s, %s, %s)\n",
                code.op.c_str(), code.larg.c_str(), code.rarg.c_str(),
                code.store.c_str());
        break;

    case STMT_READ:
        if (DEBUG)
            printf("Generating code for read.\n");
        code.op = "read";
        code.store = root->token.value;
        break;

    case STMT_WRITE:
        if (DEBUG)
            printf("Generating code for write.\n");
        code.op = "write";
        code.larg = root->token.value;
        break;
    default:
        break;
    }
    code_init_tmp(pstate);

}
static void gen_exp(
    struct syntax_tree_node_t *root,
    struct middle_code_t* pfa_code,
    struct code_gen_state_t *pstate,
    vector<struct middle_code_t> *pcode_vec)
{
    struct middle_code_t code;
    char t[10];
    switch (root->node_type)
    {
    case EXP_CONST:
    case EXP_STR:
    case EXP_ID:
        if (pfa_code->larg == ""){
            pfa_code->larg = root->token.value;
        }else if (pfa_code->rarg == ""){
            pfa_code->rarg = root->token.value;
        }else {
            assert(0);
        }
        break;
    case EXP_OP:
        switch (root->token.kind)
        {
        case TK_NOT:
            code.op = root->token.value;
            gen_exp(root->child[0], &code, pstate, pcode_vec);
            sprintf(t, "t%d", code_gen_next_tmp(pstate));
            code.store = t;
            break;
        case TK_AND:case TK_OR:
        case TK_LESS:case TK_LEQ:
        case TK_EQU:
        case TK_GREATER:case TK_GEQ:

        case TK_ADD:case TK_SUB:
        case TK_MUL:case TK_DIV:
            code.op = root->token.value;
            gen_exp(root->child[0], &code, pstate, pcode_vec);
            gen_exp(root->child[1], &code, pstate, pcode_vec);
            sprintf(t, "t%d", code_gen_next_tmp(pstate));
            code.store = t;
            break;

        default:
            assert(0);
        }
        // 回填结果存储位置
        if (pfa_code->larg == ""){
            pfa_code->larg = code.store;
        }else if (pfa_code->rarg == ""){
            pfa_code->rarg = code.store;
        }else {
            assert(0);
        }
        pcode_vec->push_back(code);
        if (DEBUG)
            printf("(%3s, %s, %s, %s)\n",
                code.op.c_str(), code.larg.c_str(), code.rarg.c_str(),
                code.store.c_str());

        break;
    default:
        assert(0);
    }
}
