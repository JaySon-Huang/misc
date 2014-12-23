#include "assert.h"
#include "tinyp_codegen.h"

int code_gen_next_index(struct code_gen_state_t *pstate);

static void gen_stmt(
    struct syntax_tree_node_t *root,
    struct middle_code_t* pmid_code,
    struct code_gen_state_t *pstate);
static char* gen_exp(
    struct syntax_tree_node_t *root,
    struct middle_code_t* pmid_code,
    struct code_gen_state_t *pstate);

void init_code_generator(
    struct middle_code_t *pmid_code,
    struct code_gen_state_t *pstate)
{
    pstate->code_line = 1;
    pstate->tmp_index = 1;
    pstate->label_index = 0;

    pmid_code->LBegin = pmid_code->LNext = pmid_code->LTrue = pmid_code->LFalse = 0;
}

int code_gen_next_index(struct code_gen_state_t *pstate)
{
    return (pstate->label_index)++;
}

char* 
generate_code(
    struct syntax_tree_node_t *root,
    struct middle_code_t* pmid_code,
    struct code_gen_state_t *pstate)
{
    struct middle_code_t E,E1;
    char *str = NULL;
    while (root != NULL){
        switch (root->node_type)
        {
        case STMT_IF:
        case STMT_REPEAT:
        case STMT_ASSIGN:
        case STMT_READ:
        case STMT_WRITE:
        case STMT_WHILE:
            E.LNext = code_gen_next_index(pstate);
            E1.LNext = pmid_code->LNext;
            gen_stmt(root, &E, pstate);
            break;
        case EXP_OP:
        case EXP_CONST:
        case EXP_ID:
        case EXP_STR:
            str = gen_exp(root, pmid_code, pstate);
            break;
        default:
            break;
        }
        root = root->sibiling;
    }
    return str;
}

static void gen_stmt(
    struct syntax_tree_node_t *root,
    struct middle_code_t* pmid_code,
    struct code_gen_state_t *pstate)
{
    char *str1;
    struct middle_code_t E,E1,E2;
    switch(root->node_type)
    {
    case STMT_IF:
        E.LTrue = code_gen_next_index(pstate);
        if(root->child[2] == NULL){
            // 没有else字句
            E.LFalse = pmid_code->LNext;
            E1.LNext = pmid_code->LNext;
            E.is_stmt = 1;
            gen_stmt(root->child[0],&E, pstate);
            if(root->child[1] != NULL)
                printf("(%d)Label L%d\n",pstate->code_line++,E.LTrue);
            generate_code(root->child[1],&E1, pstate);
        }else{
            // 有else子句
            E.LFalse = code_gen_next_index(pstate);
            E.is_stmt = 1;
            E1.LNext = pmid_code->LNext;
            E2.LNext = pmid_code->LNext;
            gen_stmt(root->child[0],&E, pstate);
            if(root->child[0]!=NULL)
                printf("(%d)Label L%d\n",pstate->code_line++,E.LTrue);
            generate_code(root->child[1],&E1, pstate);
            printf("(%d)goto L%d\n",pstate->code_line++,pmid_code->LNext);  
            if(root->child[1]!=NULL)
                printf("(%d)Label L%d\n",pstate->code_line++,E.LFalse);
            generate_code(root->child[2], &E2, pstate);
        }
        break;

    case STMT_WHILE:
        E.is_stmt=1;
        pmid_code->LBegin = code_gen_next_index(pstate);
        E.LTrue = code_gen_next_index(pstate);
        E.LFalse = pmid_code->LNext;
        E1.LNext = pmid_code->LBegin;
        printf("(%d)Label L%d\n",pstate->code_line++,pmid_code->LBegin);
        gen_stmt(root->child[0],&E, pstate);
        if(root->child[1]!=NULL)
            printf("(%d)Label L%d\n",pstate->code_line++,E.LTrue);
        generate_code(root->child[1],&E1, pstate);
        printf("(%d)goto L%d\n",pstate->code_line++,pmid_code->LBegin);
        break;

    case STMT_REPEAT:
        pmid_code->LBegin = code_gen_next_index(pstate);
        E1.LNext = code_gen_next_index(pstate);
        E.LTrue = pmid_code->LNext;
        E.LFalse = pmid_code->LBegin;
        E.is_stmt=1;
        if(root->child[0]!=NULL)
            printf("(%d)Label L%d\n",pstate->code_line++,pmid_code->LBegin);
        generate_code(root->child[0],&E1, pstate);
        printf("(%d)Label L%d\n",pstate->code_line++,E1.LNext);
        gen_stmt(root->child[1],&E, pstate);     
        break;

    case STMT_ASSIGN:
        str1 = generate_code(root->child[0], pmid_code, pstate);
        printf("(%d)%s:=%s\n", pstate->code_line++, root->token.value.c_str(), str1);
        free (str1);
        break;

    case STMT_READ:
        printf("(%d)read %s\n", pstate->code_line++, root->token.value.c_str());
        break;

    case STMT_WRITE:
        /* generate code for expression to write */
        str1 = generate_code(root->child[0], pmid_code, pstate);
        printf("(%d)write %s\n", pstate->code_line++,str1);
        free (str1);
        break;
    default:
        break;
    }
}
static char* gen_exp(
    struct syntax_tree_node_t *root,
    struct middle_code_t* pmid_code,
    struct code_gen_state_t *pstate)
{
    char *str1,*str2;
    char *str=(char*)malloc(20);
    struct middle_code_t E1, E2;
    switch (root->node_type)
    {       
    case EXP_CONST:
        sprintf(str, "%s", root->token.value.c_str());
        break;
    case EXP_STR:
    case EXP_ID:
        strcpy(str, root->token.value.c_str());
        break;
    case EXP_OP:
        switch (root->token.kind)
        {
        case TK_AND:
            E1.LTrue = code_gen_next_index(pstate);
            E1.LFalse = pmid_code->LFalse;
            E2.LTrue = pmid_code->LTrue;
            E2.LFalse = pmid_code->LFalse;
            E1.is_stmt=1;
            E2.is_stmt=1;
            str1 = generate_code(root->child[0],&E1, pstate);
            printf("(%d)Label L%d\n", pstate->code_line++, E1.LTrue);
            str2 = generate_code(root->child[1],&E2, pstate);
            break;
        case TK_OR:
            E1.LTrue = pmid_code->LTrue;
            E1.LFalse = code_gen_next_index(pstate);
            E2.LTrue = pmid_code->LTrue;
            E2.LFalse = pmid_code->LFalse;
            str1=generate_code(root->child[0], &E1, pstate);
            printf("(%d)Label L%d\n", pstate->code_line++, pmid_code->LFalse);
            str2=generate_code(root->child[1], &E2, pstate);
            break;
        case TK_NOT:
            E1.LTrue = pmid_code->LFalse;
            E1.LFalse = pmid_code->LTrue;
            str1=generate_code(root->child[0],&E1, pstate);
            str2=generate_code(root->child[1],&E2, pstate);
            break;
        case TK_GREATER:
            str1=generate_code(root->child[0],&E1, pstate);
            str2=generate_code(root->child[1],&E2, pstate);
            if(pmid_code->is_stmt==1){
                printf("(%d)if %s > %s goto L%d\n",pstate->code_line++,str1,str2,pmid_code->LTrue);
                printf("(%d)goto L%d\n",pstate->code_line++,pmid_code->LFalse);
            }else{
                printf("(%d)t%d := %s > %s\n",pstate->code_line++,pstate->tmp_index++,str1,str2);
            }
            break;
        case TK_LESS:
            str1=generate_code(root->child[0],&E1, pstate);
            str2=generate_code(root->child[1],&E2, pstate);
            if(pmid_code->is_stmt==1){
                printf("(%d)if %s < %s goto L%d\n",pstate->code_line++,str1,str2,pmid_code->LTrue);
                printf("(%d)goto L%d\n",pstate->code_line++,pmid_code->LFalse);
            }else{   
                printf("(%d)t%d := %s < %s\n",pstate->code_line++,pstate->tmp_index++,str1,str2);
            }
            break;
        case TK_EQU:
            str1=generate_code(root->child[0],&E1, pstate);
            str2=generate_code(root->child[1],&E2, pstate);
            if(pmid_code->is_stmt==1){
                printf("(%d)if %s := %s goto L%d\n",pstate->code_line++,str1,str2,pmid_code->LTrue);
                printf("(%d)goto L%d\n",pstate->code_line++,pmid_code->LFalse);
            }else{
                printf("(%d)t%d := %s =  %s\n",pstate->code_line++,pstate->tmp_index++,str1,str2);
            }
            break;
        case TK_GEQ:
            str1=generate_code(root->child[0],&E1, pstate);
            str2=generate_code(root->child[1],&E2, pstate);
            if(pmid_code->is_stmt==1){
                printf("(%d)if %s >= %s goto L%d\n",pstate->code_line++,str1,str2,pmid_code->LTrue);
                printf("(%d)goto L%d\n",pstate->code_line++,pmid_code->LFalse);
            }else{
                printf("(%d)t%d := %s >= %s\n",pstate->code_line++,pstate->tmp_index++,str1,str2);
            }
            break;
        case TK_LEQ:
            str1=generate_code(root->child[0],&E1, pstate);
            str2=generate_code(root->child[1],&E2, pstate);
            if(pmid_code->is_stmt==1){
                printf("(%d)if %s <= %s goto L%d\n",pstate->code_line++,str1,str2,pmid_code->LTrue);
                printf("(%d)goto L%d\n",pstate->code_line++,pmid_code->LFalse);
            }else{
                printf("(%d)t%d :=%s <= %s\n",pstate->code_line++,pstate->tmp_index++,str1,str2);
            }
            break;
        case TK_ADD:
            str1=generate_code(root->child[0],&E1, pstate);
            str2=generate_code(root->child[1],&E2, pstate);
            printf("(%d)t%d = %s + %s\n",pstate->code_line++,pstate->tmp_index++,str1,str2);
            break;
        case TK_SUB:
            str1=generate_code(root->child[0],&E1, pstate);
            str2=generate_code(root->child[1],&E2, pstate);
            printf("(%d)t%d = %s - %s\n",pstate->code_line++,pstate->tmp_index++,str1,str2);
            break;
        case TK_MUL:
            str1=generate_code(root->child[0],&E1, pstate);
            str2=generate_code(root->child[1],&E2, pstate);
            printf("(%d)t%d = %s * %s\n",pstate->code_line++,pstate->tmp_index++,str1,str2);
            break;
        case TK_DIV:
            str1=generate_code(root->child[0],&E1, pstate);
            str2=generate_code(root->child[1],&E2, pstate);
            printf("(%d)t%d = %s / %s\n",pstate->code_line++,pstate->tmp_index++,str1,str2);
            break;
        default:
            assert(0);
        }
        free(str1);
        free(str2);
        sprintf (str, "%s%d", "t", pstate->tmp_index);
        break;
    default:
        assert(0);
    }
    return str;
}
