#ifndef __TINYP_LEXICAL_HANDLER_H_
#define __TINYP_LEXICAL_HANDLER_H_ 

#include "tinyp_structs.h"

int _handler_start(
    int ch, 
    struct lex_state_t* plex_state,
    struct token_pair_t* ptoken_pair);

int _handler_assign(
    int ch, 
    struct lex_state_t* plex_state,
    struct token_pair_t* ptoken_pair);

int _handler_comment(
    int ch, 
    struct lex_state_t* plex_state,
    struct token_pair_t* ptoken_pair);

int _handler_done(
    int ch, 
    struct lex_state_t* plex_state,
    struct token_pair_t* ptoken_pair);

int _handler_greater(
    int ch, 
    struct lex_state_t* plex_state,
    struct token_pair_t* ptoken_pair);

int _handler_less(
    int ch, 
    struct lex_state_t* plex_state,
    struct token_pair_t* ptoken_pair);

int _handler_id(
    int ch, 
    struct lex_state_t* plex_state,
    struct token_pair_t* ptoken_pair);

int _handler_number_tmp(
    int ch, 
    struct lex_state_t* plex_state,
    struct token_pair_t* ptoken_pair);

int _handler_number(
    int ch, 
    struct lex_state_t* plex_state,
    struct token_pair_t* ptoken_pair);

int _handler_string(
    int ch, 
    struct lex_state_t* plex_state,
    struct token_pair_t* ptoken_pair);

int _handler_tran(
    int ch, 
    struct lex_state_t* plex_state,
    struct token_pair_t* ptoken_pair);

int _handler_error(
    int ch, 
    struct lex_state_t* plex_state,
    struct token_pair_t* ptoken_pair);

int _handler_number_oct(
    int ch, 
    struct lex_state_t* plex_state,
    struct token_pair_t* ptoken_pair);

int _handler_number_hex_tmp(
    int ch, 
    struct lex_state_t* plex_state,
    struct token_pair_t* ptoken_pair);

int _handler_number_hex(
    int ch, 
    struct lex_state_t* plex_state,
    struct token_pair_t* ptoken_pair);
#endif
