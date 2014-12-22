#include "tinyp_structs.h"
#include "tinyp_parser.h"
#include "syntax_tree.h"

int 
token_in(
    const set<enum Kind> *pset,
    const struct parse_state_t *pstate)
{
    if (pset->find(pstate->cur_token.kind) != pset->end()){
        return 1;
    }else{
        printf("token.kind类型不匹配\n");
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

struct syntax_tree_node_t* 
parse_program(struct parse_state_t *pstate)
{
    SymbolTable symbol_table;
    parse_declarations(&symbol_table, pstate);// 分析变量的声明
    return NULL;
    // 分析语句列表，返回值就是整个程序的语法树（不包括声明部分）
    return parse_stmt_sequence(pstate);
}

void 
parse_declarations(
    SymbolTable *ptable,
    struct parse_state_t *pstate)
{
    if (DEBUG)
        printf("parsing declarations...\n");
    while (pstate->cur_token.kind == TK_INT
        || pstate->cur_token.kind == TK_BOOL
        || pstate->cur_token.kind == TK_STR )
    {
        struct token_pair_t 
            *ptype = &(pstate->cur_token),
            *pid = NULL;

        if (DEBUG)
            token_pair_print(ptype);

        parse_state_next_token(pstate);

        while (1){
            pid = &(pstate->cur_token);
            if (token_match_one(TK_ID, pstate)){
                switch (ptype->kind)
                {
                case TK_INT:
                    ptable->update(pid, OT_VAR, VT_INT);
                    break;
                case TK_BOOL:
                    ptable->update(pid, OT_VAR, VT_BOOL);
                    break;
                case TK_STR:
                    ptable->update(pid, OT_VAR, VT_STRING);
                    break;

                default :
                    break;
                }
                if (DEBUG)
                    printf("new identifier: %s\n", pid->value.c_str());
            }

            // 遇到",",则继续解析标识符.否则跳出循环.
            if (!token_match_one(TK_COMMA, pstate)){
                break;
            }
        }
        token_match_one(TK_SEMICOLON, pstate);
    }
    if (DEBUG)
        printf("End of declarations\n");
}

struct syntax_tree_node_t* 
parse_stmt_sequence(struct parse_state_t *pstate)
{
    set<enum Kind> stmt_first;
    stmt_first.insert(TK_IF);
    stmt_first.insert(TK_WHILE);
    stmt_first.insert(TK_REPEAT);
    stmt_first.insert(TK_ID);
    stmt_first.insert(TK_READ);
    stmt_first.insert(TK_WRITE);

    struct syntax_tree_node_t *t1 = NULL, *t2 = NULL;

    struct token_pair_t first = pstate->cur_token;
    while (token_match(&stmt_first, pstate)){
        // 语句的开始符“if、while”等已由上面的match语句匹配,
        // 因此下面各语句的分析函数（if_stmt()等）不需要匹配开始符。
        switch (first.kind)
        {
        case TK_IF:
            break;
        case TK_WHILE:
            break;
        case TK_REPEAT:
            break;
        case TK_ID:
            break;
        case TK_READ:
            break;
        case TK_WRITE:
            break;

        default:
            break;
        }
        if (t1 == NULL){
            t1 = t2;
        }else{
            // 构建parse_STMT_SEQUENCE节点,t1,t2分别为左右孩子
            t1 = new_tree_node(STMT_SEQUENCE, t1, t2, NULL);
        }
    }
    return t1;
}

struct syntax_tree_node_t* 
if_stmt(struct parse_state_t *pstate)
{
    struct syntax_tree_node_t 
        *cond_exp = NULL,
        *then_stmt = NULL,
        *else_stmt = NULL;
    // cond_exp = log_or_exp(pstate);// 分析条件表达式
    then_stmt = parse_stmt_sequence(pstate);// 分析then部分
    if (pstate->cur_token.kind == TK_ELSE){
        else_stmt = parse_stmt_sequence(pstate);
    }

    // 语义检查
    if (cond_exp->val_type != VT_BOOL){
        printf("表达式的值必须为bool类型.\n");
    }

    // 构建IF_STMT节点，有3个孩子，
    // 分别是条件表达式、then语句和else语句；若没有else语句，则第三个孩子为null
    return new_tree_node(IF_STMT, cond_exp, then_stmt, else_stmt);
}

struct syntax_tree_node_t* 
factor(struct parse_state_t *pstate)
{
    set<enum Kind> factor_first;
    factor_first.insert(TK_INT);
    factor_first.insert(TK_STR);
    factor_first.insert(TK_ID);
    factor_first.insert(TK_TRUE);
    factor_first.insert(TK_FALSE);
    factor_first.insert(TK_LP);

    if (token_in(&factor_first, pstate)){
        printf("非法符号.\n");
        return NULL;
    }

    syntax_tree_node_t *node = NULL, *exp = NULL;
    if (pstate->cur_token.kind == TK_LP){
        // factor -> (logical-or-exp)
        token_match_one(TK_LP, pstate);
        // exp = log_or_exp();
        node = new_tree_node(FACTOR, exp, NULL, NULL);
        node->val_type = exp->val_type;
        token_match_one(TK_RP, pstate);
    }else{
        // factor  -> number | string | identifier | true | false 
        // node = new_tree_node(FACTOR, pstate->cur_token);// token 做为FACTOR节点的孩子
        switch (pstate->cur_token.kind)
        {
        case TK_INT:
            node->val_type = VT_INT;
            break;
        case TK_BOOL:
            node->val_type = VT_BOOL;
            break;
        case TK_STR:
            node->val_type = VT_STRING;
            break;

        default:
            break;
        }
        token_match(&factor_first, pstate);
    }
    return node;
}

