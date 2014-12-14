#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// isalpha、isdigit函数
#include <ctype.h>

#include "tinyp_structs.h"
#include "parse.h"

#define DEBUG 1

void 
_set_errdetail(
    struct parse_state_t* parse_state, 
    const char* reason)
{
    int len = strlen(reason);
    parse_state->errdetail = (char *)malloc(len*sizeof(char)+1);
    strcpy(parse_state->errdetail, reason);
}

int 
init_parser(
    const char* filename,
    struct parse_state_t* parse_state,
    struct token_pair_t** token_pair_head)
{
    parse_state->lineno = 0;
    parse_state->rowno = 0;
    parse_state->errdetail = NULL;
    parse_state->cur_state = STA_START;

    parse_state->fp = fopen(filename, "r");
    if (parse_state->fp == NULL){
        parse_state->errdetail = (char *)malloc(255*sizeof(char));
        sprintf(parse_state->errdetail, "Open file %s failed!", filename);
        return -1;
    }

    *token_pair_head = NULL;
    return 0;
}

int 
destroy_parser(
    struct parse_state_t* parse_state,
    struct token_pair_t** token_pair_head)
{
    struct token_pair_t* to_destroy = NULL;
    if (parse_state->errdetail) {
        free(parse_state->errdetail);
    }
    if (parse_state->fp != NULL) {
        fclose(parse_state->fp);
    }

    while (*token_pair_head != NULL) {
        to_destroy = *token_pair_head;
        *token_pair_head = (*token_pair_head)->next;
        free(to_destroy);
    }
    return 0;
}

int _handler_start(int ch, 
    struct parse_state_t* parse_state,
    struct token_pair_t** token_pair_head)
{
    if (isspace(ch)) {
        parse_state->cur_state = STA_START;
    }else if (isalpha(ch)) {
        // 字母
        parse_state->cur_state = STA_ID;
    }else if (isnumber(ch)){
        // 数字
        if (ch == '0'){
            parse_state->cur_state = STA_NUMBER_TMP;
        }else{
            parse_state->cur_state = STA_NUMBER;
        }
    }
    else{
        switch (ch)
        {
        case '{':// 注释块开始
            parse_state->cur_state = STA_COMMENT;
            break;
        case '\'':// 字符串开始
            parse_state->cur_state = STA_STRING;
            break;
        case ':'://赋值
            parse_state->cur_state = STA_ASSIGN;
            break;
        case '>':
            parse_state->cur_state = STA_GREATER;
            break;
        case '<':
            parse_state->cur_state = STA_LESS;
            break;
        default:
            parse_state->cur_state = STA_DONE;
            break;
        }
    }
    return 0;
}

int _handler_assign(int ch, 
    struct parse_state_t* parse_state,
    struct token_pair_t** token_pair_head)
{
    if (ch == EOF || ch == DELIMITER) {
        ungetc(ch, parse_state->fp);
        parse_state->cur_state = STA_ERROR;
        return ERROR_ASSIGN;
    }else if (ch != '='){
        parse_state->cur_state = STA_ERROR;
        return ERROR_ASSIGN;
    }else{
        parse_state->cur_state = STA_DONE;
    }
    return 0;
}

int _handler_comment(int ch, 
    struct parse_state_t* parse_state,
    struct token_pair_t** token_pair_head)
{
    if (ch == EOF) {
        parse_state->cur_state = STA_ERROR;
        ungetc(ch, parse_state->fp);
        return ERROR_COMMENT;
    }else if (ch == '}') {
        parse_state->cur_state = STA_START;
    }else {
        // 注释的内容
        // FIXME: 如何处理?需要记录吗?
    }
    return 0;
}

int _handler_done(int ch, 
    struct parse_state_t* parse_state,
    struct token_pair_t** token_pair_head)
{
    switch (ch)
    {
    case ',':
        break;
    case '+':
        break;
    case '-':
        break;
    case '*':
        break;
    case '/':
        break;
    case '(':
        break;
    case ')':
        break;
    case '=':
        break;
    case ';':
        break;
    case EOF:
        return EOF;
    case '}':
        parse_state->cur_state = STA_ERROR;
        return ERROR_COMMENT;
    default:
        parse_state->cur_state = STA_ERROR;
        return ERROR_ILLGAL_CHAR;
    }
    return 0;
}

int _handler_greater(int ch, 
    struct parse_state_t* parse_state,
    struct token_pair_t** token_pair_head)
{
    if (ch != '=') {
        ungetc(ch, parse_state->fp);
        parse_state->cur_state = STA_DONE;
    }else {
        parse_state->cur_state = STA_GEQ;
    }
    return 0;
}

int _handler_less(int ch, 
    struct parse_state_t* parse_state,
    struct token_pair_t** token_pair_head)
{
    if (ch != '='){
        ungetc(ch, parse_state->fp);
        parse_state->cur_state = STA_DONE;
    }else {
        parse_state->cur_state = STA_LEQ;
    }
    return 0;
}

int _handler_id(int ch, 
    struct parse_state_t* parse_state,
    struct token_pair_t** token_pair_head)
{
    if (ch == EOF || ch == DELIMITER) {
        ungetc(ch, parse_state->fp);
        parse_state->cur_state = STA_DONE;
    }else if (isalnum(ch)){
        // TODO: 保存值

    }else {
        ungetc(ch, parse_state->fp);
        parse_state->cur_state = STA_DONE;
    }
    return 0;
}

int _handler_number_tmp(int ch, 
    struct parse_state_t* parse_state,
    struct token_pair_t** token_pair_head)
{
    if (ch == EOF || ch == DELIMITER) {
        ungetc(ch, parse_state->fp);
        parse_state->cur_state = STA_DONE;
    }else if (isnumber(ch)) {
        ungetc(ch, parse_state->fp);
        parse_state->cur_state = STA_OCTNUMBER;
    }else if (isalpha(ch)) {
        parse_state->cur_state = STA_ERROR;
        return ERROR_NUMBER|ERROR_ILLGAL_CHAR;
    }else if (ch != 'x') {
        parse_state->cur_state = STA_ERROR;
        return ERROR_NUMBER;
    }else {
        parse_state->cur_state = STA_HEXNUMBER_TMP;
    }

    return 0;
}

int _handler_number(int ch, 
    struct parse_state_t* parse_state,
    struct token_pair_t** token_pair_head)
{
    if (ch == EOF || ch == DELIMITER){
        ungetc(ch, parse_state->fp);
        parse_state->cur_state = STA_DONE;
    }else if (isalpha(ch)){
        parse_state->cur_state = STA_ERROR;
        return ERROR_NUMBER;
    }else if (!isdigit(ch)){
        parse_state->cur_state = STA_ERROR;
        return ERROR_NUMBER|ERROR_ILLGAL_CHAR;
    }else {
        // TODO: 保存值
    }
    return 0;
}

int _handler_string(int ch, 
    struct parse_state_t* parse_state,
    struct token_pair_t** token_pair_head)
{
    if (ch == EOF || ch == '\n') {
        ungetc(ch, parse_state->fp);
        parse_state->cur_state = STA_ERROR;
        return ERROR_STR;
    }else if (ch == '\''){
        parse_state->cur_state = STA_DONE;
    }else {
        // TODO: 保存值
    }
    return 0;
}

int _handler_tran(int ch, 
    struct parse_state_t* parse_state,
    struct token_pair_t** token_pair_head)
{
    if (ch == EOF || ch == DELIMITER){
        ungetc(ch, parse_state->fp);
        parse_state->cur_state = STA_ERROR;
        return ERROR_TRAN;
    }else {
        switch (ch)
        {
        case 'a':case 'b':case 'f':case 'n':case 'r':
        case 't':case 'v':case '\\':case '\'':case '0':
            parse_state->cur_state = STA_DONE;
            break;
        default:
            parse_state->cur_state = STA_ERROR;
            return ERROR_TRAN;
        }
    }
    return 0;
}
int _handler_error(int ch, 
    struct parse_state_t* parse_state,
    struct token_pair_t** token_pair_head)
{
    if (ch == EOF || ch == DELIMITER){
        ungetc(ch, parse_state->fp);
        parse_state->cur_state = STA_DONE;
    }else if (isalnum(ch)) {
        return ERROR_ILLGAL_CHAR;
    }else{
        
    }

    return 0;
}

int parse(
    struct parse_state_t* parse_state,
    struct token_pair_t** token_pair_head)
{
    int ch;
    while (1){
        ch = get_next_char(parse_state);
        if (DEBUG)
            printf("get char:%c(%02X)\n", ch, ch);

        switch(parse_state->cur_state)
        {
        case STA_START:
            _handler_start(ch, parse_state, token_pair_head);
            break;
        case STA_ASSIGN:
            _handler_assign(ch, parse_state, token_pair_head);
            break;
        case STA_COMMENT:
            _handler_comment(ch, parse_state, token_pair_head);
            break;
        case STA_DONE:
            _handler_done(ch, parse_state, token_pair_head);
            break;
        case STA_GREATER:
            _handler_greater(ch, parse_state, token_pair_head);
            break;
        case STA_LESS:
            _handler_less(ch, parse_state, token_pair_head);
            break;
        case STA_ID:
            _handler_id(ch, parse_state, token_pair_head);
            break;
        case STA_NUMBER:
            _handler_number(ch, parse_state, token_pair_head);
            break;
        case STA_STRING:
            _handler_string(ch, parse_state, token_pair_head);
            break;
        case STA_ERROR:
            _handler_error(ch, parse_state, token_pair_head);
            break;

        default:
            fprintf(stderr, "State:%d had not been handled!\n", parse_state->cur_state);
            break;
        }
        
        
    }
    return 0;
}



int get_next_char(
    struct parse_state_t* parse_state)
{
    int ch = 0;
    if ( feof(parse_state->fp) ) {
        return EOF;
    }

    while (1) {
        ch = fgetc(parse_state->fp);
        parse_state->rowno += 1;

        if (ch == '\n') {
            parse_state->lineno += 1;
        }else {
            return ch;
        }
    }
}
