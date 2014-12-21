#ifndef __TINY_PARSE_H__
#define __TINY_PARSE_H__ 


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "tinyp_structs.h"

#include <vector>
using std::vector;

/**
  * @filename
  *    解析的文件名
  * @parse_state
  *    记录解析状态的struct
  * @ptoken_pairs
  *    解析结果vector容器
  * @return 
  *    0 :正常
  *    -1:出错,通过查看parse_state中的状态查看出错原因
  */
int 
init_parser(
    const char* filename,
    struct parse_state_t* parse_state,
    vector<struct token_pair_t> * ptoken_pairs);

/**
  * 解析结束后释放资源
  * @parse_state
  *    记录解析状态的struct
  */
int 
destroy_parser(
    struct parse_state_t* parse_state,
    vector<struct token_pair_t> * ptoken_pairs);

/**
  * 开始解析
  * @parse_state
  *    记录解析状态的struct
  * @token_pair_head
  *    解析结果链表头
  */
int parse(
    struct parse_state_t* parse_state,
    vector<struct token_pair_t> * ptoken_pairs);

/**
  * 从文件中读取下一个有效的char
  * 会更新parse_state中的行号,列号等
  * @parse_state
  *    记录解析状态的struct
  * @return
  *    正常返回0~255的char
  *    EOF代表遇到文件尾
  */
int get_next_char(
    struct parse_state_t* parse_state);

void unget_one_char(
    int ch,
    struct parse_state_t* parse_state);

struct token_pair_t* get_next_token();

#endif
