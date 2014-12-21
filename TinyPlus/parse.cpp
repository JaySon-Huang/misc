#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "tinyp_structs.h"
#include "parse.h"
#include "handlers.h"

int 
init_parser(
    const char* filename,
    struct parse_state_t* parse_state,
    vector<struct token_pair_t> * ptoken_pairs)
{
    parse_state->lineno = 1;
    parse_state->rowno = 0;
    parse_state->errdetail = NULL;
    parse_state->cur_state = STA_START;

    parse_state->fp = fopen(filename, "rb");
    if (parse_state->fp == NULL){
        parse_state->errdetail = (char *)malloc(255*sizeof(char));
        sprintf(parse_state->errdetail, "Open file %s failed!", filename);
        return -1;
    }

    return 0;
}

int 
destroy_parser(
    struct parse_state_t* parse_state,
    vector<struct token_pair_t> * ptoken_pairs)
{
    if (parse_state->errdetail) {
        free(parse_state->errdetail);
    }
    if (parse_state->fp != NULL) {
        fclose(parse_state->fp);
    }

    ptoken_pairs->clear();
    return 0;
}


void 
node_init(struct token_pair_t* pn)
{
    pn->value.clear();
}

int 
push_node(
    vector<struct token_pair_t> * ptoken_pairs, 
    struct token_pair_t* ntp)
{
    // 加到链表尾
    ptoken_pairs->push_back(*ntp);
    return 0;
}

int 
parse(
    struct parse_state_t* parse_state,
    vector<struct token_pair_t> * ptoken_pairs)
{
    int ch;
    int action = ACT_IDLE;
    struct token_pair_t token_pair;

    while (1){
        ch = get_next_char(parse_state);
        if (DEBUG)
            printf("get char:%c(0x%02X) @(%3d, %3d)\n",
             ch, ch, parse_state->lineno, parse_state->rowno);

        switch(parse_state->cur_state)
        {
        case STA_START:
            action = _handler_start(ch, parse_state, &token_pair);
            break;
        case STA_ASSIGN:
            action = _handler_assign(ch, parse_state, &token_pair);
            break;
        case STA_COMMENT:
            action = _handler_comment(ch, parse_state, &token_pair);
            break;
        case STA_DONE:
            action = _handler_done(ch, parse_state, &token_pair);
            break;
        case STA_GREATER:
            action = _handler_greater(ch, parse_state, &token_pair);
            break;
        case STA_LESS:
            action = _handler_less(ch, parse_state, &token_pair);
            break;
        case STA_ID:
            action = _handler_id(ch, parse_state, &token_pair);
            break;
        case STA_NUMBER:
            action = _handler_number(ch, parse_state, &token_pair);
            break;
        case STA_NUMBER_TMP:
            action = _handler_number_tmp(ch, parse_state, &token_pair);
            break;
        case STA_STRING:
            action = _handler_string(ch, parse_state, &token_pair);
            break;
        case STA_ERROR:
            action = _handler_error(ch, parse_state, &token_pair);
            break;

        default:
            fprintf(stderr, "State:%d had not been handled!\n", parse_state->cur_state);
            action = ACT_REPORT_ERROR;
        }
        
        switch (action)
        {
        case ACT_PUSH_NODE:
            if (DEBUG)
                printf("Node: (%d,%s)\n", token_pair.kind, token_pair.value.c_str());
            push_node(ptoken_pairs, &token_pair);
            
            node_init(&token_pair);
            break;
        case ACT_REPORT_ERROR:
            printf("In line: %d col: %d", parse_state->lineno, parse_state->rowno);
            if (parse_state->err_type & ERROR_ASSIGN){
                printf(", 赋值语句缺少\"=\"");
            }
            if (parse_state->err_type & ERROR_NUMBER){
                printf(", 数字上的错误后缀");
            }
            if (parse_state->err_type & ERROR_STR){
                printf(", 字符串常量中有换行符,缺少\"’\"");
            }
            if (parse_state->err_type & ERROR_TRAN){
                printf(", 非法的转义序列");
            }
            if (parse_state->err_type & ERROR_ILLGAL_CHAR){
                printf(", 非法字符");
            }
            if (parse_state->err_type & ERROR_COMMENT_UNEXPECTED_BRACKET){
                printf(", 在注释外找到\"}\"");
            }
            if (parse_state->err_type & ERROR_COMMENT_UNEXPECTED_EOF){
                printf(", 在注释中遇到意外的文件结束");
            }
            printf("\n");
            token_pair.value.clear();
            if (IGNORE_ERROR){
                parse_state->cur_state = STA_START;
                parse_state->err_type = 0;
                break;
            }else{
                return 1;
            }
        case ACT_EXIT:
            token_pair.kind = TK_ENDFILE;
            push_node(ptoken_pairs, &token_pair);
            if (DEBUG)
                printf("Node: (%d,%s)\n", token_pair.kind, token_pair.value.c_str());
            return 0;
        }
    }
    return 0;
}


int 
get_next_char(
    struct parse_state_t* parse_state)
{
    int ch = 0;
    if ( feof(parse_state->fp) ) {
        return EOF;
    }
    ch = fgetc(parse_state->fp);
    parse_state->rowno += 1;
    if (ch == '\r') {
        ch = fgetc(parse_state->fp);
        if (ch != '\n') {
            // printf("遇到\\r(%02X)\n", ch);
            ungetc(ch, parse_state->fp);
        }else{
            // printf("遇到\\r\\n\n");
        }
        parse_state->rowno = 0;
        parse_state->lineno += 1;
    }else if (ch == '\n') {
        parse_state->rowno = 0;
        parse_state->lineno += 1;
        // printf("遇到\\n(%02X)\n", ch);
    }
    return ch;
}

void 
unget_one_char(
    int ch,
    struct parse_state_t* parse_state)
{
    parse_state->rowno -= 1;
    ungetc(ch, parse_state->fp);
    if (ch == '\n'){
        parse_state->lineno -= 1;
    }
}
