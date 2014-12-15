#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <ctype.h>

#include "tinyp_structs.h"
#include "handlers.h"

int _handler_start(
    int ch, 
    struct parse_state_t* parse_state,
    struct token_pair_t* ntp)
{
    if (isspace(ch) || ch == '\n' || ch == '\r') {
        parse_state->cur_state = STA_START;
    }else if (isalpha(ch)) {
        // 字母
        if (DEBUG)
            printf("End Start -> ID\n");
        ntp->value[ntp->value_len++] = ch;
        parse_state->cur_state = STA_ID;
    }else if (isnumber(ch)) {
        // 数字
        if (DEBUG)
            printf("End Start -> Num (Num tmp for 0)\n");
        ntp->value[ntp->value_len++] = ch;
        if (ch == '0') {
            parse_state->cur_state = STA_NUMBER_TMP;
        }else{
            parse_state->cur_state = STA_NUMBER;
        }
    }else{
        switch (ch)
        {
        case '{':// 注释块开始
            if (DEBUG)
                printf("End Start -> Comment\n");
            parse_state->cur_state = STA_COMMENT;
            break;
        case '\'':// 字符串开始
            if (DEBUG)
                printf("End Start -> String\n");
            parse_state->cur_state = STA_STRING;
            break;
        case ':'://赋值
            if (DEBUG)
                printf("End Start -> Assign\n");
            ntp->value[ntp->value_len++] = ch;
            parse_state->cur_state = STA_ASSIGN;
            break;
        case '>':
            if (DEBUG)
                printf("End Start -> Greater\n");
            ntp->value[ntp->value_len++] = ch;
            parse_state->cur_state = STA_GREATER;
            break;
        case '<':
            if (DEBUG)
                printf("End Start -> Less\n");
            ntp->value[ntp->value_len++] = ch;
            parse_state->cur_state = STA_LESS;
            break;

        default:
            if (DEBUG)
                printf("End Start -> Done\n");
            ungetc(ch, parse_state->fp);
            parse_state->cur_state = STA_DONE;
            break;
        }
    }
    return ACT_IDLE;
}

int _handler_assign(
    int ch, 
    struct parse_state_t* parse_state,
    struct token_pair_t* ntp)
{
    if (ch == EOF || ch == DELIMITER) {
        if (DEBUG)
            printf("End Assign -> Error\n");
        ungetc(ch, parse_state->fp);
        parse_state->cur_state = STA_ERROR;
        parse_state->err_type = ERROR_ASSIGN;
    }else if (ch != '=') {
        if (DEBUG)
            printf("End Assign -> Error\n");
        parse_state->cur_state = STA_ERROR;
        parse_state->err_type = ERROR_ASSIGN;
    }else{
        if (DEBUG)
            printf("End Assign -> Done\n");
        ntp->value[ntp->value_len++] = ch;
        parse_state->cur_state = STA_DONE;
        return ACT_PUSH_NODE;
    }
    return ACT_IDLE;
}

int _handler_comment(
    int ch, 
    struct parse_state_t* parse_state,
    struct token_pair_t* ntp)
{
    if (ch == EOF) {
        if (DEBUG)
            printf("End Comment -> Error\n");
        parse_state->cur_state = STA_ERROR;
        ungetc(ch, parse_state->fp);
        parse_state->err_type = ERROR_COMMENT;
    }else if (ch == '}') {
        if (DEBUG)
            printf("End Comment -> Start\n");
        ntp->kind = TK_COMMENT;
        parse_state->cur_state = STA_START;
        return ACT_PUSH_NODE;
    }else {
        // 注释的内容
        // FIXME: 如何处理?需要记录吗?
        ntp->value[ntp->value_len++] = ch;
    }
    return ACT_IDLE;
}

int _handler_done(
    int ch, 
    struct parse_state_t* parse_state,
    struct token_pair_t* ntp)
{
    int action = ACT_IDLE;
    parse_state->cur_state = STA_START;

    if (isblank(ch) ||  ch == '\n' || ch == '\r'){
        if (DEBUG)
            printf("End Done -> Start\n");
        return ACT_IDLE;
    }else if (isalnum(ch) || ch == '>' || ch == '<' || ch == ':'){
        if (DEBUG)
            printf("End Done -> Start\n");
        ungetc(ch, parse_state->fp);
        return ACT_IDLE;
    }
    switch (ch)
    {
    case ',':
        ntp->value[ntp->value_len++] = ch;
        ntp->kind = TK_COMMA;
        action = ACT_PUSH_NODE;
        break;
    case '+':
        ntp->value[ntp->value_len++] = ch;
        ntp->kind = TK_ADD;
        action = ACT_PUSH_NODE;
        break;
    case '-':
        ntp->value[ntp->value_len++] = ch;
        ntp->kind = TK_SUB;
        action = ACT_PUSH_NODE;
        break;
    case '*':
        ntp->value[ntp->value_len++] = ch;
        ntp->kind = TK_MUL;
        action = ACT_PUSH_NODE;
        break;
    case '/':
        ntp->value[ntp->value_len++] = ch;
        ntp->kind = TK_DIV;
        action = ACT_PUSH_NODE;
        break;
    case '(':
        ntp->value[ntp->value_len++] = ch;
        ntp->kind = TK_LP;
        action = ACT_PUSH_NODE;
        break;
    case ')':
        ntp->value[ntp->value_len++] = ch;
        ntp->kind = TK_RP;
        action = ACT_PUSH_NODE;
        break;
    case '=':
        ntp->value[ntp->value_len++] = ch;
        ntp->kind = TK_EQU;
        action = ACT_PUSH_NODE;
        break;
    case ';':
        ntp->value[ntp->value_len++] = ch;
        ntp->kind = TK_SEMICOLON;
        action = ACT_PUSH_NODE;
        break;

    case EOF:
        action = ACT_EXIT;
        break;

    case '}':
        if (DEBUG)
            printf("End Done -> Error\n");
        parse_state->cur_state = STA_ERROR;
        parse_state->err_type = ERROR_COMMENT;
        action = ACT_IDLE;
    default:
        if (DEBUG)
            printf("End Done -> Error\n");
        parse_state->cur_state = STA_ERROR;
        parse_state->err_type = ERROR_ILLGAL_CHAR;
        action = ACT_IDLE;
    }
    if (DEBUG){
        switch (action)
        {
        case ACT_PUSH_NODE:
            printf("Get an operator.\n");
            printf("End Done -> Start\n");
            break;
        case ACT_EXIT:
            printf("Get EOF. Exit.\n");
            break;
        }
    }
    return action;
}

int _handler_number_tmp(
    int ch, 
    struct parse_state_t* parse_state,
    struct token_pair_t* ntp)
{
    if (ch == EOF || ch == DELIMITER || isblank(ch)) {
        if (DEBUG)
            printf("End Num tmp -> Done\n");
        ungetc(ch, parse_state->fp);
        parse_state->cur_state = STA_DONE;
        ntp->kind = TK_NUM;
        return ACT_PUSH_NODE;
    }else if (isnumber(ch)) {
        if (DEBUG)
            printf("End Num tmp -> Num Oct\n");
        ungetc(ch, parse_state->fp);
        parse_state->cur_state = STA_OCTNUMBER;
    }else if (!isalpha(ch)) {
        if (DEBUG)
            printf("End Num tmp -> Error\n");
        parse_state->cur_state = STA_ERROR;
        parse_state->err_type = ERROR_NUMBER|ERROR_ILLGAL_CHAR;
    }else if (ch != 'x') {
        if (DEBUG)
            printf("End Num tmp -> Error\n");
        parse_state->cur_state = STA_ERROR;
        parse_state->err_type = ERROR_NUMBER;
    }else {
        if (DEBUG)
            printf("End Num tmp -> Num Hex\n");
        parse_state->cur_state = STA_HEXNUMBER_TMP;
    }

    return ACT_IDLE;
}

int _handler_number(
    int ch, 
    struct parse_state_t* parse_state,
    struct token_pair_t* ntp)
{
    if (ch == EOF || ch == DELIMITER || isblank(ch)) {
        if (DEBUG)
            printf("End Num -> Done\n");
        ungetc(ch, parse_state->fp);
        parse_state->cur_state = STA_DONE;
        ntp->kind = TK_NUM;
        return ACT_PUSH_NODE;
    }else if (isalpha(ch)) {
        if (DEBUG)
            printf("End Num -> Error\n");
        parse_state->cur_state = STA_ERROR;
        parse_state->err_type = ERROR_NUMBER;
    }else if (!isdigit(ch)) {
        if (DEBUG)
            printf("End Num -> Error\n");
        parse_state->cur_state = STA_ERROR;
        parse_state->err_type = ERROR_NUMBER|ERROR_ILLGAL_CHAR;
    }else {
        // TODO: 保存值
        ntp->value[ntp->value_len++] = ch;
    }
    return ACT_IDLE;
}

int _handler_greater(
    int ch, 
    struct parse_state_t* parse_state,
    struct token_pair_t* ntp)
{
    if (ch != '=') {
        if (DEBUG)
            printf("End Greater -> Done\n");
        ungetc(ch, parse_state->fp);
        parse_state->cur_state = STA_DONE;
        ntp->kind = TK_GREATER;
        return ACT_PUSH_NODE;
    }else {
        if (DEBUG)
            printf("Greater -> Greater or Euqal\n");
        ntp->value[ntp->value_len++] = ch;
        parse_state->cur_state = STA_GEQ;
        return ACT_IDLE;
    }
    return ACT_IDLE;
}

int _handler_less(
    int ch, 
    struct parse_state_t* parse_state,
    struct token_pair_t* ntp)
{
    if (ch != '=') {
        if (DEBUG)
            printf("End Less -> Done\n");
        ungetc(ch, parse_state->fp);
        parse_state->cur_state = STA_DONE;
        ntp->kind = TK_LESS;
        return ACT_PUSH_NODE;
    }else {
        if (DEBUG)
            printf("Less -> Less or Euqal\n");
        ntp->value[ntp->value_len++] = ch;
        parse_state->cur_state = STA_LEQ;
    }
    return ACT_IDLE;
}

/**
 *   关键字:
 *   TK_TRUE, TK_FALSE, TK_OR,
 *   TK_AND, TK_NOT, TK_INT,
 *   TK_BOOL, TK_STRING, TK_WHILE,
 *   TK_DO, TK_IF, TK_THEN,
 *   TK_ELSE, TK_END, TK_REPEAT,
 *   TK_UNTIL, TK_READ, TK_WRITE,
 */
void _key_or_id(struct token_pair_t* ntp)
{
    if (0 == strcmp(ntp->value, "true")){
        ntp->kind = TK_TRUE;
    }else if (0 == strcmp(ntp->value, "false")){
        ntp->kind = TK_FALSE;
    }else if (0 == strcmp(ntp->value, "or")){
        ntp->kind = TK_OR;
    }else if (0 == strcmp(ntp->value, "and")){
        ntp->kind = TK_AND;
    }else if (0 == strcmp(ntp->value, "not")){
        ntp->kind = TK_NOT;
    }else if (0 == strcmp(ntp->value, "int")){
        ntp->kind = TK_INT;
    }else if (0 == strcmp(ntp->value, "bool")){
        ntp->kind = TK_BOOL;
    }else if (0 == strcmp(ntp->value, "string")){
        ntp->kind = TK_STRING;
    }else if (0 == strcmp(ntp->value, "while")){
        ntp->kind = TK_WHILE;
    }else if (0 == strcmp(ntp->value, "do")){
        ntp->kind = TK_DO;
    }else if (0 == strcmp(ntp->value, "if")){
        ntp->kind = TK_IF;
    }else if (0 == strcmp(ntp->value, "then")){
        ntp->kind = TK_THEN;
    }else if (0 == strcmp(ntp->value, "else")){
        ntp->kind = TK_ELSE;
    }else if (0 == strcmp(ntp->value, "end")){
        ntp->kind = TK_END;
    }else if (0 == strcmp(ntp->value, "repeat")){
        ntp->kind = TK_REPEAT;
    }else if (0 == strcmp(ntp->value, "until")){
        ntp->kind = TK_UNTIL;
    }else if (0 == strcmp(ntp->value, "read")){
        ntp->kind = TK_READ;
    }else if (0 == strcmp(ntp->value, "write")){
        ntp->kind = TK_WRITE;
    }else {
        ntp->kind = TK_ID;
    }
    
}

int _handler_id(
    int ch, 
    struct parse_state_t* parse_state,
    struct token_pair_t* ntp)
{
    if (ch == EOF || ch == DELIMITER) {
        if (DEBUG)
            printf("End ID -> Done\n");
        ungetc(ch, parse_state->fp);
        parse_state->cur_state = STA_DONE;
        _key_or_id(ntp);
        return ACT_PUSH_NODE;
    }else if (isalnum(ch)) {
        // 保存值
        ntp->value[ntp->value_len++] = ch;
    }else {
        if (DEBUG)
            printf("End ID -> Done\n");
        ungetc(ch, parse_state->fp);
        parse_state->cur_state = STA_DONE;
        _key_or_id(ntp);
        return ACT_PUSH_NODE;
    }
    return ACT_IDLE;
}

int _handler_string(
    int ch, 
    struct parse_state_t* parse_state,
    struct token_pair_t* ntp)
{
    if (ch == EOF || ch == '\n') {
        if (DEBUG)
            printf("String -> Error\n");
        ungetc(ch, parse_state->fp);
        parse_state->cur_state = STA_ERROR;
        parse_state->err_type = ERROR_STR;
    }else if (ch == '\'') {
        if (DEBUG)
            printf("End String -> Done\n");
        parse_state->cur_state = STA_DONE;
        ntp->kind = TK_STRING;
        return ACT_PUSH_NODE;
    }else {
        // 保存值
        ntp->value[ntp->value_len++] = ch;
    }
    return ACT_IDLE;
}

int _handler_tran(
    int ch, 
    struct parse_state_t* parse_state,
    struct token_pair_t* ntp)
{
    if (ch == EOF || ch == DELIMITER) {
        if (DEBUG)
            printf("Trans -> Error\n");
        ungetc(ch, parse_state->fp);
        parse_state->cur_state = STA_ERROR;
        parse_state->err_type = ERROR_TRAN;
    }else {
        switch (ch)
        {
        case 'a':case 'b':case 'f':case 'n':case 'r':
        case 't':case 'v':case '\\':case '\'':case '0':
            if (DEBUG)
                printf("End Trans\n");
            ntp->value[ntp->value_len++] = ch;
            parse_state->cur_state = STA_DONE;
            break;
        
        default:
            if (DEBUG)
                printf("Trans -> Error\n");
            parse_state->cur_state = STA_ERROR;
            parse_state->err_type = ERROR_TRAN;
            break;
        }
    }
    return ACT_IDLE;
}
int _handler_error(
    int ch, 
    struct parse_state_t* parse_state,
    struct token_pair_t* ntp)
{
    if (ch == EOF || ch == DELIMITER) {
        ungetc(ch, parse_state->fp);
        parse_state->cur_state = STA_DONE;
        return ACT_PUSH_NODE;
    }else if (isalnum(ch)) {
        parse_state->err_type = ERROR_ILLGAL_CHAR;
    }else{

    }

    return ACT_REPORT_ERROR;
}
