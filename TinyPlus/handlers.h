#ifndef __TINY_HANDLER_H_
#define __TINY_HANDLER_H_ 

#include "tinyp_structs.h"

int _handler_start(
    int ch, 
    struct parse_state_t* parse_state,
    struct token_pair_t* ntp);

int _handler_assign(
    int ch, 
    struct parse_state_t* parse_state,
    struct token_pair_t* ntp);

int _handler_comment(
    int ch, 
    struct parse_state_t* parse_state,
    struct token_pair_t* ntp);

int _handler_done(
    int ch, 
    struct parse_state_t* parse_state,
    struct token_pair_t* ntp);

int _handler_greater(
    int ch, 
    struct parse_state_t* parse_state,
    struct token_pair_t* ntp);

int _handler_less(
    int ch, 
    struct parse_state_t* parse_state,
    struct token_pair_t* ntp);

int _handler_id(
    int ch, 
    struct parse_state_t* parse_state,
    struct token_pair_t* ntp);

int _handler_number_tmp(
    int ch, 
    struct parse_state_t* parse_state,
    struct token_pair_t* ntp);

int _handler_number(
    int ch, 
    struct parse_state_t* parse_state,
    struct token_pair_t* ntp);

int _handler_string(
    int ch, 
    struct parse_state_t* parse_state,
    struct token_pair_t* ntp);

int _handler_tran(
    int ch, 
    struct parse_state_t* parse_state,
    struct token_pair_t* ntp);

int _handler_error(
    int ch, 
    struct parse_state_t* parse_state,
    struct token_pair_t* ntp);

int _handler_number_oct(
    int ch, 
    struct parse_state_t* parse_state,
    struct token_pair_t* ptoken_pair);

int _handler_number_hex_tmp(
    int ch, 
    struct parse_state_t* parse_state,
    struct token_pair_t* ptoken_pair);

int _handler_number_hex(
    int ch, 
    struct parse_state_t* parse_state,
    struct token_pair_t* ptoken_pair);
#endif
