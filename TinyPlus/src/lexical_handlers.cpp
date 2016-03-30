#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <ctype.h>

#include "lexical_handlers.h"
#include "tinyp_lexer.h"
#include "tinyp_structs.h"

int _handler_start(int ch, struct lex_state_t* plex_state,
                   token_pair_t* ptoken_pair) {
    if (isspace(ch) || ch == '\n' || ch == '\r') {
        plex_state->cur_state = STA_START;
    } else if (isalpha(ch)) {
        // 字母
        if (DEBUG) printf("End Start -> ID\n");
        ptoken_pair->value += ch;
        plex_state->cur_state = STA_ID;
    } else if (isdigit(ch)) {
        // 数字
        if (DEBUG) printf("End Start -> Num (Num tmp for 0)\n");
        ptoken_pair->value += ch;
        if (ch == '0') {
            plex_state->cur_state = STA_NUMBER_TMP;
        } else {
            plex_state->cur_state = STA_NUMBER;
        }
    } else {
        switch (ch) {
            case '{':  // 注释块开始
                if (DEBUG) printf("End Start -> Comment\n");
                plex_state->cur_state = STA_COMMENT;
                break;
            case '\'':  // 字符串开始
                if (DEBUG) printf("End Start -> String\n");
                plex_state->cur_state = STA_STRING;
                break;
            case ':':  //赋值
                if (DEBUG) printf("End Start -> Assign\n");
                ptoken_pair->value += ch;
                plex_state->cur_state = STA_ASSIGN;
                break;
            case '>':
                if (DEBUG) printf("End Start -> Greater\n");
                ptoken_pair->value += ch;
                plex_state->cur_state = STA_GREATER;
                break;
            case '<':
                if (DEBUG) printf("End Start -> Less\n");
                ptoken_pair->value += ch;
                plex_state->cur_state = STA_LESS;
                break;

            default:
                if (DEBUG) printf("End Start -> Done\n");
                unget_one_char(ch, plex_state);
                plex_state->cur_state = STA_DONE;
                break;
        }
    }
    return ACT_IDLE;
}

int _handler_assign(int ch, struct lex_state_t* plex_state,
                    token_pair_t* ptoken_pair) {
    if (ch == EOF || ch == DELIMITER) {
        if (DEBUG) printf("End Assign -> Error\n");
        unget_one_char(ch, plex_state);
        plex_state->cur_state = STA_ERROR;
        plex_state->err_type |= ERROR_ASSIGN;
        return ACT_REPORT_ERROR;
    } else if (ch != '=') {
        if (DEBUG) printf("End Assign -> Error\n");
        plex_state->cur_state = STA_ERROR;
        plex_state->err_type |= ERROR_ASSIGN;
        return ACT_REPORT_ERROR;
    } else {
        if (DEBUG) printf("End Assign -> Done\n");
        ptoken_pair->value += ch;
        ptoken_pair->kind = TK_ASSIGN;
        plex_state->cur_state = STA_DONE;
        return ACT_PUSH_NODE;
    }
    return ACT_IDLE;
}

int _handler_comment(int ch, struct lex_state_t* plex_state,
                     token_pair_t* ptoken_pair) {
    if (ch == EOF) {
        if (DEBUG) printf("End Comment -> Error\n");
        plex_state->cur_state = STA_ERROR;
        unget_one_char(ch, plex_state);
        plex_state->err_type |= ERROR_COMMENT_UNEXPECTED_EOF;
        return ACT_REPORT_ERROR;
    } else if (ch == '}') {
        if (DEBUG) printf("End Comment -> Start\n");
        ptoken_pair->kind = TK_COMMENT;
        plex_state->cur_state = STA_START;
        return ACT_COMMENT;
    } else {
        // 注释的内容
        ptoken_pair->value += ch;
    }
    return ACT_IDLE;
}

int _handler_done(int ch, struct lex_state_t* plex_state,
                  token_pair_t* ptoken_pair) {
    int action = ACT_IDLE;
    plex_state->cur_state = STA_START;

    if (isspace(ch) || ch == '\n' || ch == '\r') {
        if (DEBUG) printf("End Done -> Start\n");
        return ACT_IDLE;
    } else if (isalnum(ch) || ch == '>' || ch == '<' || ch == ':') {
        if (DEBUG) printf("End Done -> Start\n");
        unget_one_char(ch, plex_state);
        return ACT_IDLE;
    }
    switch (ch) {
        case ',':
            ptoken_pair->value += ch;
            ptoken_pair->kind = TK_COMMA;
            action = ACT_PUSH_NODE;
            break;
        case '+':
            ptoken_pair->value += ch;
            ptoken_pair->kind = TK_ADD;
            action = ACT_PUSH_NODE;
            break;
        case '-':
            ptoken_pair->value += ch;
            ptoken_pair->kind = TK_SUB;
            action = ACT_PUSH_NODE;
            break;
        case '*':
            ptoken_pair->value += ch;
            ptoken_pair->kind = TK_MUL;
            action = ACT_PUSH_NODE;
            break;
        case '/':
            ptoken_pair->value += ch;
            ptoken_pair->kind = TK_DIV;
            action = ACT_PUSH_NODE;
            break;
        case '(':
            ptoken_pair->value += ch;
            ptoken_pair->kind = TK_LP;
            action = ACT_PUSH_NODE;
            break;
        case ')':
            ptoken_pair->value += ch;
            ptoken_pair->kind = TK_RP;
            action = ACT_PUSH_NODE;
            break;
        case '=':
            ptoken_pair->value += ch;
            ptoken_pair->kind = TK_EQU;
            action = ACT_PUSH_NODE;
            break;
        case ';':
            ptoken_pair->value += ch;
            ptoken_pair->kind = TK_SEMICOLON;
            action = ACT_PUSH_NODE;
            break;

        case EOF:
            action = ACT_EXIT;
            break;

        case '}':
            if (DEBUG) printf("End Done -> Error\n");
            plex_state->cur_state = STA_ERROR;
            plex_state->err_type |= ERROR_COMMENT_UNEXPECTED_BRACKET;
            action = ACT_REPORT_ERROR;
        default:
            if (DEBUG) printf("End Done -> Error\n");
            plex_state->cur_state = STA_ERROR;
            plex_state->err_type |= ERROR_ILLGAL_CHAR;
            action = ACT_REPORT_ERROR;
    }
    if (DEBUG) {
        switch (action) {
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

int _handler_number_tmp(int ch, struct lex_state_t* plex_state,
                        token_pair_t* ptoken_pair) {
    if (ch == EOF || ch == DELIMITER || isspace(ch) || ch == '\n' ||
        ch == '=' || ch == '<' || ch == '>' || ch == ')' || ch == '*' ||
        ch == '/' || ch == '-' || ch == '+') {
        if (DEBUG) printf("End Num tmp -> Done\n");
        unget_one_char(ch, plex_state);
        plex_state->cur_state = STA_DONE;
        ptoken_pair->kind = TK_NUM;
        return ACT_PUSH_NODE;
    } else if (isdigit(ch)) {
        if (DEBUG) printf("End Num tmp -> Num Oct\n");
        unget_one_char(ch, plex_state);
        plex_state->cur_state = STA_OCTNUMBER;
    } else if (!isalpha(ch)) {
        if (DEBUG) printf("End Num tmp -> Error\n");
        plex_state->cur_state = STA_ERROR;
        plex_state->err_type |= ERROR_NUMBER | ERROR_ILLGAL_CHAR;
        return ACT_REPORT_ERROR;
    } else if (ch != 'x') {
        if (DEBUG) printf("End Num tmp -> Error\n");
        plex_state->cur_state = STA_ERROR;
        plex_state->err_type |= ERROR_NUMBER;
        return ACT_REPORT_ERROR;
    } else {
        if (DEBUG) printf("End Num tmp -> Num Hex\n");
        ptoken_pair->value += ch;
        plex_state->cur_state = STA_HEXNUMBER_TMP;
    }

    return ACT_IDLE;
}

int _handler_number(int ch, struct lex_state_t* plex_state,
                    token_pair_t* ptoken_pair) {
    if (ch == EOF || ch == DELIMITER || isspace(ch) || ch == '\n' ||
        ch == '=' || ch == '<' || ch == '>' || ch == ')' || ch == '*' ||
        ch == '/' || ch == '-' || ch == '+') {
        if (DEBUG) printf("End Num -> Done\n");
        unget_one_char(ch, plex_state);
        plex_state->cur_state = STA_DONE;
        ptoken_pair->kind = TK_NUM;
        return ACT_PUSH_NODE;
    } else if (isalpha(ch)) {
        if (DEBUG) printf("End Num -> Error\n");
        plex_state->cur_state = STA_ERROR;
        plex_state->err_type |= ERROR_NUMBER;
        return ACT_REPORT_ERROR;
    } else if (!isdigit(ch)) {
        if (DEBUG) printf("End Num -> Error\n");
        plex_state->cur_state = STA_ERROR;
        plex_state->err_type |= ERROR_NUMBER | ERROR_ILLGAL_CHAR;
        return ACT_REPORT_ERROR;
    } else {
        // TODO: 保存值
        ptoken_pair->value += ch;
    }
    return ACT_IDLE;
}

int _handler_number_oct(int ch, struct lex_state_t* plex_state,
                        token_pair_t* ptoken_pair) {
    if (ch == EOF || ch == DELIMITER || isspace(ch) || ch == '\n' ||
        ch == '=' || ch == '<' || ch == '>' || ch == ')' || ch == '*' ||
        ch == '/' || ch == '-' || ch == '+') {
        if (DEBUG) printf("End OCT -> Done\n");
        unget_one_char(ch, plex_state);
        plex_state->cur_state = STA_DONE;
        ptoken_pair->kind = TK_OCTNUM;
        return ACT_PUSH_NODE;
    } else if (isalpha(ch)) {
        plex_state->err_type |= ERROR_NUMBER;
        plex_state->cur_state = STA_ERROR;
        return ACT_REPORT_ERROR;
    } else if (!isdigit(ch)) {
        plex_state->err_type |= ERROR_ILLGAL_CHAR;
        plex_state->cur_state = STA_ERROR;
        return ACT_REPORT_ERROR;
    } else if (ch >= '0' && ch <= '7') {
        ptoken_pair->value += ch;
        return ACT_IDLE;
    } else {
        plex_state->cur_state = STA_ERROR;
        plex_state->err_type |= ERROR_OCT;
        return ACT_REPORT_ERROR;
    }
    return ACT_IDLE;
}

int _handler_number_hex_tmp(int ch, struct lex_state_t* plex_state,
                            token_pair_t* ptoken_pair) {
    if (ch == EOF || ch == DELIMITER || isspace(ch) || ch == '\n' ||
        ch == '=' || ch == '<' || ch == '>' || ch == ')' || ch == '*' ||
        ch == '/' || ch == '-' || ch == '+') {
        if (DEBUG) printf("End HEX TMP -> Done\n");
        unget_one_char(ch, plex_state);
        plex_state->err_type |= ERROR_HEX;
        plex_state->cur_state = STA_ERROR;
        return ACT_REPORT_ERROR;
    } else if (!isalnum(ch)) {
        plex_state->cur_state = STA_ERROR;
        plex_state->err_type |= ERROR_ILLGAL_CHAR | ERROR_HEX;
        return ACT_REPORT_ERROR;
    } else if (!isxdigit(ch)) {
        plex_state->cur_state = STA_ERROR;
        plex_state->err_type |= ERROR_HEX;
        return ACT_REPORT_ERROR;
    } else {
        if (DEBUG) printf("End HEX TMP -> HEX\n");
        plex_state->cur_state = STA_HEXNUMBER;
        ptoken_pair->value += ch;
        return ACT_IDLE;
    }
}

int _handler_number_hex(int ch, struct lex_state_t* plex_state,
                        token_pair_t* ptoken_pair) {
    if (ch == EOF || ch == DELIMITER || isspace(ch) || ch == '\n' ||
        ch == '=' || ch == '<' || ch == '>' || ch == ')' || ch == '*' ||
        ch == '/' || ch == '-' || ch == '+') {
        if (DEBUG) printf("End HEX -> Done\n");
        unget_one_char(ch, plex_state);
        plex_state->cur_state = STA_DONE;
        ptoken_pair->kind = TK_HEXNUM;
        return ACT_PUSH_NODE;
    } else if (!isalnum(ch)) {
        plex_state->cur_state = STA_ERROR;
        plex_state->err_type |= ERROR_ILLGAL_CHAR | ERROR_HEX;
        return ACT_REPORT_ERROR;
    } else if (!isxdigit(ch)) {
        plex_state->cur_state = STA_ERROR;
        plex_state->err_type |= ERROR_HEX;
        return ACT_REPORT_ERROR;
    } else {
        ptoken_pair->value += ch;
        return ACT_IDLE;
    }
}

int _handler_greater(int ch, struct lex_state_t* plex_state,
                     token_pair_t* ptoken_pair) {
    if (ch != '=') {
        if (DEBUG) printf("End Greater -> Done\n");
        unget_one_char(ch, plex_state);
        plex_state->cur_state = STA_DONE;
        ptoken_pair->kind = TK_GREATER;
        return ACT_PUSH_NODE;
    } else {
        if (DEBUG) printf("Greater -> Greater or Euqal\n");
        ptoken_pair->value += ch;
        plex_state->cur_state = STA_DONE;
        ptoken_pair->kind = TK_GEQ;
        return ACT_PUSH_NODE;
    }
    return ACT_IDLE;
}

int _handler_less(int ch, struct lex_state_t* plex_state,
                  token_pair_t* ptoken_pair) {
    if (ch != '=') {
        if (DEBUG) printf("End Less -> Done\n");
        unget_one_char(ch, plex_state);
        plex_state->cur_state = STA_DONE;
        ptoken_pair->kind = TK_LESS;
        return ACT_PUSH_NODE;
    } else {
        if (DEBUG) printf("Less -> Less or Euqal\n");
        ptoken_pair->value += ch;
        plex_state->cur_state = STA_DONE;
        ptoken_pair->kind = TK_LEQ;
        return ACT_PUSH_NODE;
    }
    return ACT_IDLE;
}

int _handler_id(int ch, struct lex_state_t* plex_state,
                token_pair_t* ptoken_pair) {
    if (ch == EOF || ch == DELIMITER) {
        if (DEBUG) printf("End ID -> Done\n");
        unget_one_char(ch, plex_state);
        plex_state->cur_state = STA_DONE;
        token_pair_kind_key(ptoken_pair);
        return ACT_PUSH_NODE;
    } else if (isalnum(ch)) {
        // 保存值
        ptoken_pair->value += ch;
    } else {
        if (DEBUG) printf("End ID -> Done\n");
        unget_one_char(ch, plex_state);
        plex_state->cur_state = STA_DONE;
        token_pair_kind_key(ptoken_pair);
        return ACT_PUSH_NODE;
    }
    return ACT_IDLE;
}

int _handler_string(int ch, struct lex_state_t* plex_state,
                    token_pair_t* ptoken_pair) {
    if (ch == EOF || ch == '\n') {
        if (DEBUG) printf("String -> Error\n");
        unget_one_char(ch, plex_state);
        plex_state->cur_state = STA_ERROR;
        plex_state->err_type |= ERROR_STR;
        return ACT_REPORT_ERROR;
    } else if (ch == '\'') {
        if (DEBUG) printf("End String -> Done\n");
        plex_state->cur_state = STA_DONE;
        ptoken_pair->kind = TK_STRING;
        return ACT_PUSH_NODE;
    } else {
        // 保存值
        ptoken_pair->value += ch;
    }
    return ACT_IDLE;
}

int _handler_tran(int ch, struct lex_state_t* plex_state,
                  token_pair_t* ptoken_pair) {
    if (ch == EOF || ch == DELIMITER) {
        if (DEBUG) printf("Trans -> Error\n");
        unget_one_char(ch, plex_state);
        plex_state->cur_state = STA_ERROR;
        plex_state->err_type |= ERROR_TRAN;
        return ACT_REPORT_ERROR;
    } else {
        switch (ch) {
            case 'a':
            case 'b':
            case 'f':
            case 'n':
            case 'r':
            case 't':
            case 'v':
            case '\\':
            case '\'':
            case '0':
                if (DEBUG) printf("End Trans\n");
                ptoken_pair->value += ch;
                plex_state->cur_state = STA_DONE;
                break;

            default:
                if (DEBUG) printf("Trans -> Error\n");
                plex_state->cur_state = STA_ERROR;
                plex_state->err_type |= ERROR_TRAN;
                return ACT_REPORT_ERROR;
        }
    }
    return ACT_IDLE;
}

int _handler_error(int ch, struct lex_state_t* plex_state,
                   token_pair_t* ptoken_pair) {
    if (ch == EOF || ch == DELIMITER) {
        unget_one_char(ch, plex_state);
        plex_state->cur_state = STA_DONE;
        return ACT_PUSH_NODE;
    } else if (isalnum(ch)) {
        plex_state->err_type |= ERROR_ILLGAL_CHAR;
    } else {
    }

    return ACT_REPORT_ERROR;
}
