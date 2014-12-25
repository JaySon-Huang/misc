#ifndef __TINY_STRUCTS_H__
#define __TINY_STRUCTS_H__ 

// FILE* 定义
#include <stdio.h>

#include <string>
using std::string;
#include <vector>
using std::vector;

enum Kind {
    // 关键字
    TK_TRUE, TK_FALSE, TK_OR,
    TK_AND, TK_NOT, TK_INT,
    TK_BOOL, TK_STR, TK_WHILE,
    TK_DO, TK_IF, TK_THEN,
    TK_ELSE, TK_END, TK_REPEAT,
    TK_UNTIL, TK_READ, TK_WRITE,
    // 操作符
    TK_GREATER, // >
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
    TK_LESS, // <
    TK_EQU, // =

    TK_COMMENT, //注释
    TK_ENDFILE, // 文件尾
    TK_STRING, // 字符串
    TK_ID, // 标识符
    TK_NUM, // 十进制数
    TK_OCTNUM, // 八进制数
    TK_HEXNUM, // 十六进制数
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
    STA_GREATER, //
    STA_STRING, // 解析字符串
    STA_TRAN, // 转义字符
    STA_DONE, // 
    STA_ERROR, // 错误
};
enum Error {
    ERROR_COMMENT_UNEXPECTED_EOF=0x100,
    ERROR_COMMENT_UNEXPECTED_BRACKET=0x200,
    ERROR_NUMBER=0x400,
    ERROR_ILLGAL_CHAR=0x800,
    ERROR_ASSIGN=0x1000,
    ERROR_TRAN=0x2000,
    ERROR_STR=0x4000,
    ERROR_OCT=0x1,
    ERROR_HEX=0x2,
};

enum Action {
    ACT_IDLE, // 空
    ACT_EXIT, // 退出解析
    ACT_PUSH_NODE,// 把结点接到链表中
    ACT_REPORT_ERROR, // 报告错误
    ACT_COMMENT,// 解析到注释
};
// 保存解析出来的token串的链表结点
struct token_pair_t{
    enum Kind kind;// token 类型
    string value;// token 值
};

void 
token_pair_print(struct token_pair_t* ptoken_pair);
void 
token_pair_kind_key(struct token_pair_t* ptoken_pair);
void
token_pair_copy(struct token_pair_t *dest, const struct token_pair_t *src);

// 保存当前解析状态,出错原因
struct lex_state_t{
    FILE* fp;
    int lineno;// 行号
    int rowno;// 列号
    char* errdetail;// 出错时保存详细信息
    int err_type;//错误类型
    enum State cur_state;
};
struct parse_state_t{
    struct token_pair_t cur_token;
    vector<struct token_pair_t> *ptoken_pairs;
    int token_pos;
};
enum ObjectType {
    OT_FUN,   // 函数
    OT_VAR,   // 变量
    OT_CONST, // 常量
};

enum ValueType {
    VT_INT,    // 整形
    VT_BOOL,   // 布尔类型
    VT_STRING, // 字符串类型
};

enum NodeType {
    NT_PROGRAM,    // 程序（开始符号）节点
    STMT_SEQUENCE, // 语句列表节点

    STMT_IF,       // 条件语句节点
    STMT_REPEAT,   // repeat语句节点
    STMT_ASSIGN,   // 赋值语句节点
    STMT_READ,     // read语句节点
    STMT_WRITE,    // write语句节点
    STMT_WHILE,    // while语句节点

    EXP_GTR,       // 大于表达式节点
    EXP_GEQ,       // 大于等于表达式节点
    EXP_LSS,       // 小于表达式节点
    EXP_LEQ,       // 小于等于表达式节点
    EXP_LOG_OR,    // 逻辑或表达式节点
    EXP_LOG_AND,   // 逻辑与表达式节点
    EXP_LOG_NOT,   // 逻辑非表达式节点
    EXP_ADD,       // 加法表达式节点
    EXP_SUB,       // 减法表达式节点
    EXP_MUL,       // 乘法表达式节点
    EXP_DIV,       // 除法表达式节点

    EXP_OP,
    EXP_CONST,
    EXP_ID,
    EXP_STR,

    NT_FACTOR,     // 原子节点
};

struct symbol_t {
    struct token_pair_t token; // token
    enum ObjectType obj_type; // 符号对象类型
    enum ValueType val_type; //值类型
    // int addr;  // 地址
    // int size;  // 字节数
    // int level; // 层次
};

struct syntax_tree_node_t {
    enum NodeType node_type;// 节点类型
    enum ValueType val_type;// 节点值类型
    struct syntax_tree_node_t* child[3];// 子节点
    struct syntax_tree_node_t* sibiling;// 兄弟节点
    struct token_pair_t token; // 当节点为FACTOR类型时该成员才有效

    int LTrue, LFalse;
};

#define DELIMITER ';'
#define DEBUG 1
#define IGNORE_ERROR 0
#define DETAIL 1

#endif
