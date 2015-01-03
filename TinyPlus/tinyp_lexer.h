#ifndef __TINYP_LEXER_H__
#define __TINYP_LEXER_H__ 


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "tinyp_structs.h"

#include <vector>
using std::vector;

/**
  * @filename
  *    解析的文件名
  * @plex_state
  *    记录词法分析状态的struct的指针
  * @ptoken_pairs
  *    解析结果vector容器
  * @return 
  *    0 :正常
  *    -1:出错,通过查看plex_state中的状态查看出错原因
  */
int 
init_lexer(
    const char* filename,
    struct lex_state_t* plex_state,
    vector<token_pair_t> * ptoken_pairs);

/**
  * 解析结束后释放资源
  * @plex_state
  *    记录词法分析状态的struct的指针
  * @ptoken_pairs
  *    解析结果vector容器
  */
int 
destroy_lexer(
    struct lex_state_t* plex_state,
    vector<token_pair_t> * ptoken_pairs);

/**
  * 开始解析
  * @plex_state
  *    记录词法分析状态的struct的指针
  * @ptoken_pairs
  *    解析结果vector容器
  */
int lexical(
    struct lex_state_t* plex_state,
    vector<token_pair_t> * ptoken_pairs);

/**
  * 从文件中读取下一个有效的char
  * 会更新plex_state中的行号,列号等
  * @plex_state
  *    记录词法分析状态的struct的指针
  * @return
  *    正常返回0~255的char
  *    EOF代表遇到文件尾
  */
int get_next_char(
    struct lex_state_t* plex_state);

void unget_one_char(
    int ch,
    struct lex_state_t* plex_state);

#endif
