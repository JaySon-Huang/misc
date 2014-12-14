#ifndef __TINY_CONSTANTS_H__
#define __TINY_CONSTANTS_H__ 

// FILE* 定义
#include <stdio.h>

enum Kind {
    // 关键字
    TK_TRUE, TK_FALSE, TK_OR,
    TK_AND, TK_NOT, TK_INT,
    TK_BOOL, TK_STRING, TK_WHILE,
    TK_DO, TK_IF, TK_THEN,
    TK_ELSE, TK_END, TK_REPEAT,
    TK_UNTIL, TK_READ, TK_WRITE,
    // 操作符
    TK_GTR, // >
    TK_LEQ, // <=
    TK_GEQ, // >=
    TK_COMMA, // ,
    TK_QUOTE, // '
    TK_SEMICOLON, // ;
    TK_ASSIGN,// :=
    TK_ADD, // +
    TK_SUB, // -
    TK_MUL, // *
    TK_DIV, // /
    TK_LP, // (
    TK_RP, // )
    TK_LSS, // <
    TK_EQU, // =

    TK_ENDFILE, // 文件尾
};
enum State {
    STA_START, // 空白状态
    STA_COMMENT, // 解析注释
    STA_NUMBER_TMP, //
    STA_NUMBER, // 解析数字
    STA_OCTNUMBER, // 解析8进制数字
    STA_HEXNUMBER_TMP, //
    STA_HEXNUMBER, // 解析16进制数字
    STA_ID, // 解析标识符
    STA_ASSIGN, // 解析赋值
    STA_LESS, // 
    STA_LEQ, //
    STA_GREATER, //
    STA_GEQ, //
    STA_STRING, // 解析字符串
    STA_TRAN, // 转义字符
    STA_DONE, // 
    STA_ERROR, // 错误
};
enum Error {
    ERROR_COMMENT=0x100,
    ERROR_NUMBER=0x200,
    ERROR_ILLGAL_CHAR=0x400,
    ERROR_ASSIGN=0x800,
    ERROR_TRAN=0x1000,
    ERROR_STR=0x2000,
};
// 保存解析出来的token串的链表结点
struct token_pair_t{
    enum Kind kind;// token 类型
    char *value;// token 值
    struct token_pair_t* next;// 下一个结点
};

// 保存当前解析状态,出错原因
struct parse_state_t{
    FILE* fp;
    int lineno;// 行号
    int rowno;// 列号
    char* errdetail;// 出错时保存详细信息
    enum State cur_state;
};

#define DELIMITER ';'

#endif
