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
/*
static void gen_exp(
    struct syntax_tree_node_t *root,
    struct middle_code_t* pfa_code,
    struct code_gen_state_t *pstate,
    vector<struct middle_code_t> *pcode_vec);
*/
static void gen_rval(
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

void code_print(struct middle_code_t *pcode)
{
    if (pcode->islabel){
        printf("%s\n", pcode->store.c_str());
    }else{
    printf("%4d :(%3s, %4s, %4s, %5s)\n",
        pcode->lineno,
        pcode->op.c_str(), 
        pcode->larg.c_str(), pcode->rarg.c_str(),
        pcode->store.c_str());
    }
}

static void code_fill_arg(
    struct middle_code_t *pcode,
    string *arg)
{
    if (pcode->larg == ""){
        pcode->larg.assign(*arg);
    }else if (pcode->rarg == ""){
        pcode->rarg.assign(*arg);
    }else {
        assert(0);
    }
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
            gen_rval(root, pfa_code, pstate, pcode_vec);
            break;
        default:
            break;
        }
        root = root->sibiling;
    }
}
/*
static void gen_stmt_if(
    struct syntax_tree_node_t *root,
    struct middle_code_t* pfa_code,
    struct code_gen_state_t *pstate,
    vector<struct middle_code_t> *pcode_vec)
{
    struct middle_code_t code;
    if (DEBUG)
        printf("Generating code for if.\n");
    gen_exp(root->child[0], &code, pstate, pcode_vec);

}*/

static void gen_stmt(
    struct syntax_tree_node_t *root,
    struct middle_code_t* pfa_code,
    struct code_gen_state_t *pstate,
    vector<struct middle_code_t> *pcode_vec)
{
    char t[20];
    memset(t, 0, sizeof(t));
    struct middle_code_t code, label, label2;
    code.islabel = 0;label.islabel = label2.islabel = 1;
    // label.is_label = true;
    switch(root->node_type)
    {
    case STMT_IF:
        /*
        gen_stmt_if(root, pfa_code, pstate, pcode_vec);
        break;
        */
        
        if (DEBUG)
            printf("Generating code for if.\n");
        code.op = "jnz";
        sprintf(t, "Label %2d", code_gen_next_index(pstate));
        code.store.assign(t);
        gen_rval(root->child[0], &code, pstate, pcode_vec);
        code.lineno = pstate->code_line++;
        pcode_vec->push_back(code);
        if (DEBUG)
            code_print(&code);
        generate_code(root->child[1], NULL, pstate, pcode_vec);
        
        // label记着if失败的位置
        label.store.assign(code.store);

        if (root->child[2] == NULL){
            pcode_vec->push_back(label);
            if (DEBUG)
                printf("%s\n", label.store.c_str());
        }else{
            // 执行完if为真的情况,跳出if-else
            code.op = "jmp";
            code.larg = "";code.rarg = "";
            sprintf(t, "Label %2d", code_gen_next_index(pstate));
            code.store.assign(t);
            code.lineno = pstate->code_line++;
            pcode_vec->push_back(code);

            if (DEBUG)
                code_print(&code);
            // 处理else
            pcode_vec->push_back(label);
            if (DEBUG)
                printf("%s\n", label.store.c_str());
            generate_code(root->child[2], NULL, pstate, pcode_vec);
            label.store.assign(code.store);
            // 完全跳出的位置
            pcode_vec->push_back(label);
            if (DEBUG)
                printf("%s\n", label.store.c_str());
        }
        break;
        

    case STMT_WHILE:
        if (DEBUG)
            printf("Generating code for while.\n");
        // 循环的位置
        sprintf(t, "Label %2d", code_gen_next_index(pstate));
        label.store.assign(t);
        if (DEBUG)
            printf("%s\n", label.store.c_str());
        pcode_vec->push_back(label);
        // 跳出的位置
        sprintf(t, "Label %2d", code_gen_next_index(pstate));
        label2.store.assign(t);

        // 条件为假时,需要跳到while外
        code.op = "jz";
        code.store.assign(label2.store);
        gen_rval(root->child[0], &code, pstate, pcode_vec);
        code.lineno = pstate->code_line++;
        pcode_vec->push_back(code);
        if (DEBUG)
            code_print(&code);

        // 循环主体
        generate_code(root->child[1], NULL, pstate, pcode_vec);

        code.op = "jmp";
        code.larg = "";code.rarg="";
        code.store.assign(label.store);
        code.lineno = pstate->code_line++;
        pcode_vec->push_back(code);
        if (DEBUG)
            code_print(&code);

        pcode_vec->push_back(label2);
        if (DEBUG)
            printf("%s\n", label2.store.c_str());
        break;

    case STMT_REPEAT:
        if (DEBUG)
            printf("Generating code for repeat.\n");
        sprintf(t, "Label %2d", code_gen_next_index(pstate));
        label.store.assign(t);
        pcode_vec->push_back(label);
        if (DEBUG)
            printf("%s\n", label.store.c_str());

        generate_code(root->child[0], NULL, pstate, pcode_vec);
        code.op = "jz";
        code.store.assign(label.store);
        gen_rval(root->child[1], &code, pstate, pcode_vec);
        code.lineno = pstate->code_line++;
        pcode_vec->push_back(code);
        if (DEBUG)
            code_print(&code);
        pcode_vec->push_back(label);
        
        break;

    case STMT_ASSIGN:
        if (DEBUG)
            printf("Generating code for assign.\n");
        code.op = ":=";
        code.store = root->token.value;
        gen_rval(root->child[0], &code, pstate, pcode_vec);
        // 
        code_init_tmp(pstate);
        code.lineno = pstate->code_line++;
        pcode_vec->push_back(code);
        if (DEBUG)
            code_print(&code);
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
    

}
/*
static void gen_exp(
    struct syntax_tree_node_t *root,
    struct middle_code_t* pcode,
    struct code_gen_state_t *pstate,
    vector<struct middle_code_t> *pcode_vec)
{
    middle_code_t code1, code2;
    if (root->child[0]->node_type == EXP_ID
        || root->child[0]->node_type == EXP_CONST){
        switch (root->token.kind)
        {
        case TK_LESS:
            code1.op = "jlt";
            break;
        case TK_LEQ:
            code1.op = "jle";
            break;
        case TK_EQU:
            code1.op = "jeq";
            break;
        case TK_GREATER:
            code1.op = "jgt";
            break;
        case TK_GEQ:
            code1.op = "jge";
            break;
        default:
            assert(0);
        }
        code1.larg = root->child[0]->token.value;
        code1.rarg = root->child[1]->token.value;

        root->LTrue = code1.lineno = pstate->code_line++;
        root->LFalse = code2.lineno = pstate->code_line++;
    }else{
        gen_exp(root->child[0], &code1, pstate, pcode_vec);
        int next = pstate->code_line;
        gen_exp(root->child[1], &code1, pstate, pcode_vec);

    }


}
*/
static void gen_rval(
    struct syntax_tree_node_t *root,
    struct middle_code_t* pfa_code,
    struct code_gen_state_t *pstate,
    vector<struct middle_code_t> *pcode_vec)
{
    struct middle_code_t code;
    code.islabel = 0;
    char t[10];
    switch (root->node_type)
    {
    case EXP_CONST:
    case EXP_STR:
    case EXP_ID:
        code_fill_arg(pfa_code, &root->token.value);
        break;
    case EXP_OP:
        switch (root->token.kind)
        {
        case TK_NOT:
            code.op = root->token.value;
            gen_rval(root->child[0], &code, pstate, pcode_vec);
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
            gen_rval(root->child[0], &code, pstate, pcode_vec);
            gen_rval(root->child[1], &code, pstate, pcode_vec);
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
        code.lineno = pstate->code_line++;
        pcode_vec->push_back(code);
        if (DEBUG)
            code_print(&code);

        break;
    default:
        assert(0);
    }
}
