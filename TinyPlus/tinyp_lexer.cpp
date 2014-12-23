#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "tinyp_structs.h"
#include "tinyp_lexer.h"
#include "lexical_handlers.h"

int 
init_lexer(
    const char* filename,
    struct lex_state_t* plex_state,
    vector<struct token_pair_t> * ptoken_pairs)
{
    plex_state->lineno = 1;
    plex_state->rowno = 0;
    plex_state->errdetail = NULL;
    plex_state->cur_state = STA_START;

    plex_state->fp = fopen(filename, "rb");
    if (plex_state->fp == NULL){
        plex_state->errdetail = (char *)malloc(255*sizeof(char));
        sprintf(plex_state->errdetail, "Open file %s failed!", filename);
        return -1;
    }

    return 0;
}

int 
destroy_lexer(
    struct lex_state_t* plex_state,
    vector<struct token_pair_t> * ptoken_pairs)
{
    if (plex_state->errdetail) {
        free(plex_state->errdetail);
    }
    if (plex_state->fp != NULL) {
        fclose(plex_state->fp);
    }

    ptoken_pairs->clear();
    return 0;
}


void 
node_init(struct token_pair_t* pn)
{
    pn->value.clear();
}

void 
action_report_error(struct lex_state_t* plex_state)
{
    printf("In line: %d col: %d", plex_state->lineno, plex_state->rowno);
    if (plex_state->err_type & ERROR_ASSIGN){
        printf(", 赋值语句缺少\"=\"");
    }
    if (plex_state->err_type & ERROR_NUMBER){
        printf(", 数字上的错误后缀");
    }
    if (plex_state->err_type & ERROR_STR){
        printf(", 字符串常量中有换行符,缺少\"'\"");
    }
    if (plex_state->err_type & ERROR_TRAN){
        printf(", 非法的转义序列");
    }
    if (plex_state->err_type & ERROR_ILLGAL_CHAR){
        printf(", 非法字符");
    }
    if (plex_state->err_type & ERROR_COMMENT_UNEXPECTED_BRACKET){
        printf(", 在注释外找到\"}\"");
    }
    if (plex_state->err_type & ERROR_COMMENT_UNEXPECTED_EOF){
        printf(", 在注释中遇到意外的文件结束");
    }
    if (plex_state->err_type & ERROR_HEX){
        printf(", 十六进制常量中发现非法字符");
    }
    if (plex_state->err_type & ERROR_OCT){
        printf(", 八进制常量中发现非法字符");
    }
    printf("\n");
}

int 
parse(
    struct lex_state_t* plex_state,
    vector<struct token_pair_t> * ptoken_pairs)
{
    int ch;
    int action = ACT_IDLE;
    struct token_pair_t token_pair;

    while (1){
        ch = get_next_char(plex_state);
        if (DEBUG)
            printf("get char:%c(0x%02X) @(%3d, %3d)\n",
             ch, ch, plex_state->lineno, plex_state->rowno);

        switch(plex_state->cur_state)
        {
        case STA_START:
            action = _handler_start(ch, plex_state, &token_pair);
            break;
        case STA_ASSIGN:
            action = _handler_assign(ch, plex_state, &token_pair);
            break;
        case STA_COMMENT:
            action = _handler_comment(ch, plex_state, &token_pair);
            break;
        case STA_DONE:
            action = _handler_done(ch, plex_state, &token_pair);
            break;
        case STA_GREATER:
            action = _handler_greater(ch, plex_state, &token_pair);
            break;
        case STA_LESS:
            action = _handler_less(ch, plex_state, &token_pair);
            break;
        case STA_ID:
            action = _handler_id(ch, plex_state, &token_pair);
            break;
        case STA_NUMBER:
            action = _handler_number(ch, plex_state, &token_pair);
            break;
        case STA_NUMBER_TMP:
            action = _handler_number_tmp(ch, plex_state, &token_pair);
            break;
        case STA_OCTNUMBER:
            action = _handler_number_oct(ch, plex_state, &token_pair);
            break;
        case STA_HEXNUMBER_TMP:
            action = _handler_number_hex_tmp(ch, plex_state, &token_pair);
            break;
        case STA_HEXNUMBER:
            action = _handler_number_hex(ch, plex_state, &token_pair);
            break;
        case STA_STRING:
            action = _handler_string(ch, plex_state, &token_pair);
            break;
        case STA_ERROR:
            action = _handler_error(ch, plex_state, &token_pair);
            break;

        default:
            fprintf(stderr, "State:%d had not been handled!\n", plex_state->cur_state);
            action = ACT_REPORT_ERROR;
        }
        
        switch (action)
        {
        case ACT_PUSH_NODE:
            if (DEBUG)
                printf("Node: (%d,%s)\n", token_pair.kind, token_pair.value.c_str());
            ptoken_pairs->push_back(token_pair);
            node_init(&token_pair);
            break;
        case ACT_COMMENT:
            if (DEBUG)
                printf("Comment: %s\n", token_pair.value.c_str());
            node_init(&token_pair);
            break;
        case ACT_REPORT_ERROR:
            action_report_error(plex_state);
            token_pair.value.clear();
            if (IGNORE_ERROR){
                plex_state->cur_state = STA_START;
                plex_state->err_type = 0;
                break;
            }else{
                return 1;
            }
        case ACT_EXIT:
            token_pair.kind = TK_ENDFILE;
            ptoken_pairs->push_back(token_pair);
            if (DEBUG)
                printf("Node: (%d,%s)\n", token_pair.kind, token_pair.value.c_str());
            return 0;
        }
    }
    return 0;
}

int 
get_next_char(
    struct lex_state_t* plex_state)
{
    int ch = 0;
    if ( feof(plex_state->fp) ) {
        return EOF;
    }
    ch = fgetc(plex_state->fp);
    plex_state->rowno += 1;
    if (ch == '\r') {
        ch = fgetc(plex_state->fp);
        if (ch != '\n') {
            // printf("遇到\\r(%02X)\n", ch);
            ungetc(ch, plex_state->fp);
        }else{
            // printf("遇到\\r\\n\n");
        }
        plex_state->rowno = 0;
        plex_state->lineno += 1;
    }else if (ch == '\n') {
        plex_state->rowno = 0;
        plex_state->lineno += 1;
        // printf("遇到\\n(%02X)\n", ch);
    }
    return ch;
}

void 
unget_one_char(
    int ch,
    struct lex_state_t* plex_state)
{
    plex_state->rowno -= 1;
    ungetc(ch, plex_state->fp);
    if (ch == '\n'){
        plex_state->lineno -= 1;
        plex_state->rowno = 0;
    }
}
