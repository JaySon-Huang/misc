#include "tinyp_structs.h"
#include "tinyp_parser.h"
#include "syntax_tree.h"

#include <assert.h>

int
token_in(
    const set<enum Kind> *pset,
    const struct parse_state_t *pstate)
{
    if (pset->find(pstate->cur_token.kind) != pset->end()){
        return 1;
    }else{
        return 0;
    }
}

int
token_match(
    const set<enum Kind> *pset,
    struct parse_state_t *pstate)
{
    if (pset->find(pstate->cur_token.kind) != pset->end()){
        parse_state_next_token(pstate);
        return 1;
    }else{
        printf("token.kind类型不匹配\n");
        return 0;
    }
}

int
token_match_one(
    const enum Kind kind,
    struct parse_state_t *pstate)
{
    if (pstate->cur_token.kind == kind){
        parse_state_next_token(pstate);
        return 1;
    }else{
        printf("token.kind类型不匹配\n");
        return 0;
    }
}

void
parse_state_next_token(struct parse_state_t *pstate)
{
    pstate->cur_token = pstate->ptoken_pairs->at(pstate->token_pos++);
}

void
init_parser(
            struct parse_state_t *pstate,
            vector<struct token_pair_t> *ptoken_pairs)
{
    pstate->ptoken_pairs = ptoken_pairs;
    pstate->cur_token = ptoken_pairs->at(0);
    pstate->token_pos = 1;
}

/* program -> declarations stmt-sequence
 */
struct syntax_tree_node_t*
parse_program(struct parse_state_t *pstate)
{
    
    
    SymbolTable symbol_table;
    parse_declarations(&symbol_table, pstate);// 分析变量的声明
    // 分析语句列表，返回值就是整个程序的语法树（不包括声明部分）
    return parse_stmt_sequence(pstate);
}

/* declarations -> decl;declarations|e
 * decl -> type-specifier varlist
 * type-specifier -> int|bool|string
 * varlist -> identifier[, varlist]
 */
void
parse_declarations(
    SymbolTable *ptable,
    struct parse_state_t *pstate)
{
    if (DEBUG)
        printf("parsing declarations...\n");
    // type-specifier -> int|bool|string
    while (pstate->cur_token.kind == TK_INT
           || pstate->cur_token.kind == TK_BOOL
           || pstate->cur_token.kind == TK_STR )
    {
        struct token_pair_t type,id;
        token_pair_copy(&type, &pstate->cur_token);
        
        if (DEBUG)
            token_pair_print(&type);
        
        parse_state_next_token(pstate);
        // varlist -> identifier[, varlist]
        while (1){
            token_pair_copy(&id, &pstate->cur_token);
            if (DEBUG)
                token_pair_print(&id);
            if (token_match_one(TK_ID, pstate)){
                switch (type.kind)
                {
                    case TK_INT:
                        ptable->update(&id, OT_VAR, VT_INT);
                        break;
                    case TK_BOOL:
                        ptable->update(&id, OT_VAR, VT_BOOL);
                        break;
                    case TK_STR:
                        ptable->update(&id, OT_VAR, VT_STRING);
                        break;
                        
                    default :// 不可能到达
                        break;
                }
                if (DEBUG)
                    printf("new identifier: %s\n", id.value.c_str());
            }
            
            // 遇到",",则继续解析标识符.否则跳出循环.
            if (pstate->cur_token.kind == TK_COMMA){
                token_match_one(TK_COMMA, pstate);
            }else{
                break;
            }
        }
        token_match_one(TK_SEMICOLON, pstate);
    }
    if (DEBUG)
        printf("End of declarations\n");
}

/* stmt-sequenct -> statement[; stmt-sequence]
 */
struct syntax_tree_node_t*
parse_stmt_sequence(struct parse_state_t *pstate)
{
    if (DEBUG)
        printf("Parsing statement sequence..\n");
    struct syntax_tree_node_t *t = parse_statement(pstate);
    token_match_one(TK_SEMICOLON, pstate);
    struct syntax_tree_node_t *p = t;
    while (pstate->cur_token.kind != TK_ENDFILE
           && pstate->cur_token.kind != TK_END
           && pstate->cur_token.kind != TK_ELSE
           && pstate->cur_token.kind != TK_UNTIL)
    {
        struct syntax_tree_node_t *q;
        q = parse_statement(pstate);
        token_match_one(TK_SEMICOLON, pstate);
        if (q != NULL){
            if (t == NULL){
                t = p = q;
            }else{
                p->sibiling = q;
                p = q;
            }
        }
    }
    if (DEBUG)
        printf("End of statement sequence.\n");
    return t;
}

/* statement -> if-stmt|repeat-stmt|while-stmt
 *              |assign-stmt|read-stmt|write-stmt
 */
struct syntax_tree_node_t*
parse_statement(struct parse_state_t *pstate)
{
    if (DEBUG)
        printf("Parsing one statement.\n");
    struct syntax_tree_node_t *t = NULL;
    switch (pstate->cur_token.kind)
    {
        case TK_IF:
            if (DEBUG)
                printf("statement -> if-stmt\n");
            t = parse_if_stmt(pstate);
            break;
        case TK_WHILE:
            if (DEBUG)
                printf("statement -> while-stmt\n");
            t = parse_while_stmt(pstate);
            break;
        case TK_REPEAT:
            if (DEBUG)
                printf("statement -> repeat-stmt\n");
            t = parse_repeat_stmt(pstate);
            break;
        case TK_ID:
            if (DEBUG)
                printf("statement -> assign-stmt\n");
            t = parse_assign_stmt(pstate);
            break;
        case TK_READ:
            if (DEBUG)
                printf("statement -> read-stmt\n");
            t = parse_read_stmt(pstate);
            break;
        case TK_WRITE:
            if (DEBUG)
                printf("statement -> write-stmt\n");
            t = parse_write_stmt(pstate);
            break;
            
        default:
            printf("Unexpected Token while parsing statement:");
            token_pair_print(&pstate->cur_token);
            assert(0);
            break;
    }
    if (DEBUG)
        printf("End of one statement.\n\n");
    return t;
}

/* if-stmt -> if logical-or-exp then stmt-sequence
 *             [else stmt-sequence] end
 */
struct syntax_tree_node_t*
parse_if_stmt(struct parse_state_t *pstate)
{
    struct syntax_tree_node_t *t = new_stmt_node(STMT_IF);
    token_match_one(TK_IF, pstate);
    if (t != NULL){
        t->child[0] = parse_bool_exp(pstate);
    }
    token_match_one(TK_THEN, pstate);
    if (t != NULL){
        t->child[1] = parse_stmt_sequence(pstate);
    }
    if (pstate->cur_token.kind == TK_ELSE){
        token_match_one(TK_ELSE, pstate);
        if (t != NULL){
            t->child[2] = parse_stmt_sequence(pstate);
        }
    }
    token_match_one(TK_END, pstate);
    if (DEBUG)
        printf("End of if-stmt\n");
    return t;
}

/* while-stmt -> while logical-or-exp do stmt-sequence end
 */
struct syntax_tree_node_t*
parse_while_stmt(struct parse_state_t *pstate)
{
    struct syntax_tree_node_t *t = new_stmt_node(STMT_WHILE);
    token_match_one(TK_WHILE, pstate);
    if (t != NULL){
        t->child[0] = parse_bool_exp(pstate);
    }
    token_match_one(TK_DO, pstate);
    if (t != NULL){
        t->child[1] = parse_stmt_sequence(pstate);
    }
    token_match_one(TK_END, pstate);
    if (DEBUG)
        printf("End of while-stmt\n");
    return t;
}

/* repeat-stmt -> repeat stmt-sequence until logical-or-exp
 */
struct syntax_tree_node_t*
parse_repeat_stmt(struct parse_state_t *pstate)
{
    struct syntax_tree_node_t *t = new_stmt_node(STMT_REPEAT);
    token_match_one(TK_REPEAT, pstate);
    if (t != NULL){
        t->child[0] = parse_stmt_sequence(pstate);
    }
    token_match_one(TK_UNTIL, pstate);
    if (t != NULL){
        t->child[1] = parse_bool_exp(pstate);
    }
    if (DEBUG)
        printf("End of repeat-stmt\n");
    return t;
}

/* read-stmt -> read identifier
 */
struct syntax_tree_node_t*
parse_read_stmt(struct parse_state_t *pstate)
{
    struct syntax_tree_node_t *t = new_stmt_node(STMT_READ);
    token_match_one(TK_READ, pstate);
    if (t != NULL && pstate->cur_token.kind == TK_ID){
        token_pair_copy(&t->token, &pstate->cur_token);
    }
    token_match_one(TK_ID, pstate);
    if (DEBUG)
        printf("End of read-stmt\n");
    return t;
}

/* write-stmt -> write logical-or-exp
 */
struct syntax_tree_node_t*
parse_write_stmt(struct parse_state_t *pstate)
{
    struct syntax_tree_node_t *t = new_stmt_node(STMT_WRITE);
    token_match_one(TK_WRITE, pstate);
    if (t != NULL){
        t->child[0] = parse_exp(pstate);
    }
    if (DEBUG)
        printf("End of write-stmt\n");
    return t;
}

/* assign-stmt -> identifier := logical-or-exp
 */
struct syntax_tree_node_t*
parse_assign_stmt(struct parse_state_t *pstate)
{
    if (DEBUG)
        printf("assign-stmt -> identifier := logical-or-exp\n");
    struct syntax_tree_node_t *t = new_stmt_node(STMT_ASSIGN);
    if (t != NULL && pstate->cur_token.kind == TK_ID){
        token_pair_copy(&t->token, &pstate->cur_token);
    }
    token_match_one(TK_ID, pstate);
    token_match_one(TK_ASSIGN, pstate);
    if (t != NULL){
        t->child[0] = parse_exp(pstate);
    }
    if (DEBUG)
        printf("End of assign-stmt\n");
    return t;
}

/* bool-exp -> logical-or-exp
 */
struct syntax_tree_node_t*
parse_bool_exp(struct parse_state_t *pstate)
{
    if (DEBUG)
        printf("Parsing Bool expression\n");
    struct syntax_tree_node_t *t = parse_or_exp(pstate);
    if (DEBUG)
        printf("End of Bool expression\n");
    return t;
}

/* logical-or-exp -> logical-and-exp[or logical-or-exp]
 */
struct syntax_tree_node_t*
parse_or_exp(struct parse_state_t *pstate)
{
    struct syntax_tree_node_t *t = parse_and_exp(pstate);
    struct syntax_tree_node_t *p = t;
    while (t != NULL && pstate->cur_token.kind == TK_OR){
        struct syntax_tree_node_t *q = NULL;
        p = new_exp_node(EXP_OP);
        token_pair_copy(&p->token, &pstate->cur_token);
        token_match_one(TK_OR, pstate);
        p->child[0] = t;
        t = p;
        q = parse_and_exp(pstate);
        if (q != NULL){
            t->child[1] = q;
        }
    }
    return t;
}

/* logical-and-exp -> comparison-exp[and logical-and-exp]
 */
struct syntax_tree_node_t*
parse_and_exp(struct parse_state_t *pstate)
{
    struct syntax_tree_node_t *t = parse_not_exp(pstate);
    struct syntax_tree_node_t *p = t;
    while (t != NULL && pstate->cur_token.kind == TK_AND){
        struct syntax_tree_node_t *q = NULL;
        p = new_exp_node(EXP_OP);
        token_pair_copy(&p->token, &pstate->cur_token);
        token_match_one(TK_AND, pstate);
        p->child[0] = t;
        t = p;
        q = parse_not_exp(pstate);
        if (q != NULL){
            t->child[1] = q;
        }
    }
    return t;
}

/*
 */
struct syntax_tree_node_t*
parse_not_exp(struct parse_state_t *pstate)
{
    struct syntax_tree_node_t *t = NULL, *p=NULL;
    switch (pstate->cur_token.kind)
    {
        case TK_NOT:
            t = new_exp_node(EXP_OP);
            token_pair_copy(&t->token, &pstate->cur_token);
            token_match_one(TK_NOT, pstate);
            p = parse_exp(pstate);
            if (p != NULL){
                t->child[0] = p;
            }
            break;
        case TK_TRUE:case TK_FALSE:
            // 直接字面布尔值
            t = new_exp_node(EXP_CONST);
            token_pair_copy(&t->token, &pstate->cur_token);
            token_match_one(pstate->cur_token.kind, pstate);
            break;
        case TK_LP:case TK_ID:
        case TK_NUM:case TK_HEXNUM:case TK_OCTNUM:
            t = parse_compare_exp(pstate);
            break;
            
        default:
            assert(0);
            break;
    }
    return t;
}

/*
 */
struct syntax_tree_node_t*
parse_exp(struct parse_state_t *pstate)
{
    if (DEBUG)
        printf("Parsing expression\n");
    struct syntax_tree_node_t *t = NULL;
    switch (pstate->cur_token.kind)
    {
        case TK_NUM:case TK_HEXNUM:case TK_OCTNUM:
        case TK_ID:case TK_LP:case TK_NOT:
        case TK_TRUE:case TK_FALSE:
            t = parse_bool_exp(pstate);
            break;
        case TK_STRING:
            t = parse_string_exp(pstate);
            break;
            
        default:
            printf("Unexpected Token:");
            token_pair_print(&pstate->cur_token);
            assert(0);
            break;
    }
    if (DEBUG)
        printf("End of expression.\n");
    return t;
}

/* comparison-exp -> add-exp [comparison-op comparison-exp]
 */
struct syntax_tree_node_t*
parse_compare_exp(struct parse_state_t *pstate)
{
    struct syntax_tree_node_t *t = parse_arithmetic_exp(pstate);
    enum Kind kind = pstate->cur_token.kind;
    if (kind == TK_LESS || kind == TK_GREATER || kind == TK_EQU
        || kind == TK_GEQ || kind == TK_LEQ)
    {
        struct syntax_tree_node_t *p = new_exp_node(EXP_OP);
        if (p != NULL){
            p->child[0] = t;
            token_pair_copy(&p->token, &pstate->cur_token);
            t = p;
            token_match_one(pstate->cur_token.kind, pstate);
            if (t != NULL){
                t->child[1] = parse_arithmetic_exp(pstate);
            }
        }
    }
    return t;
}

/* 处理 加减
 */
struct syntax_tree_node_t*
parse_arithmetic_exp(struct parse_state_t *pstate)
{
    struct syntax_tree_node_t *t = parse_term(pstate);
    while (pstate->cur_token.kind == TK_ADD
           || pstate->cur_token.kind == TK_SUB)
    {
        struct syntax_tree_node_t *p = new_exp_node(EXP_OP);
        if (p != NULL){
            p->child[0] = t;
            token_pair_copy(&p->token, &pstate->cur_token);
            t = p;
            token_match_one(pstate->cur_token.kind, pstate);
            p->child[1] = parse_term(pstate);
        }
    }
    return t;
}

/* 处理 乘除
 */
struct syntax_tree_node_t*
parse_term(struct parse_state_t *pstate){
    struct syntax_tree_node_t *t = parse_factor(pstate);
    while (pstate->cur_token.kind == TK_MUL
           || pstate->cur_token.kind == TK_DIV)
    {
        struct syntax_tree_node_t *p = new_exp_node(EXP_OP);
        if (p != NULL){
            p->child[0] = t;
            token_pair_copy(&p->token, &pstate->cur_token);
            t = p;
            token_match_one(pstate->cur_token.kind, pstate);
            p->child[1] = parse_factor(pstate);
        }
    }
    return t;
}

/* factor -> number|string|identifier
 *            |true|false|(logical-or-exp)
 */
struct syntax_tree_node_t*
parse_factor(struct parse_state_t *pstate)
{
    struct syntax_tree_node_t *t = NULL;
    switch(pstate->cur_token.kind)
    {
        case TK_NUM:case TK_OCTNUM:case TK_HEXNUM:
            t = new_exp_node(EXP_CONST);
            if (t != NULL 
                && (pstate->cur_token.kind == TK_NUM
                    || pstate->cur_token.kind == TK_OCTNUM
                    || pstate->cur_token.kind == TK_HEXNUM) )
            {
                token_pair_copy(&t->token, &pstate->cur_token);
            }
            token_match_one(pstate->cur_token.kind, pstate);
            break;
        case TK_ID:// -> 
            t = new_exp_node(EXP_ID);
            if (t != NULL && pstate->cur_token.kind == TK_ID){
                token_pair_copy(&t->token, &pstate->cur_token);
            }
            token_match_one(TK_ID, pstate);
            break;
        case TK_LP:// -> (logical-or-exp)
            token_match_one(TK_LP, pstate);
            t = parse_exp(pstate);
            token_match_one(TK_RP, pstate);
            break;
            
        default:
            assert(0);
            break;
    }
    return t;
}

/*
 */
struct syntax_tree_node_t* 
parse_string_exp(struct parse_state_t *pstate)
{
    if (DEBUG)
        printf("expression string\n");
    struct syntax_tree_node_t *t = new_exp_node(EXP_STR);
    if (t != NULL){
        token_pair_copy(&t->token, &pstate->cur_token);
        token_match_one(TK_STRING, pstate);
    }
    return t;
}
