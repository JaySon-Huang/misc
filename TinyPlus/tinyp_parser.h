#ifndef __TINYP_PARSER_H__
#define __TINYP_PARSER_H__ 
// 语法分析

#include "tinyp_structs.h"
#include "symboltable.h"
#include <vector>
using std::vector;
#include <set>
using std::set;
#include <string>
using std::string;

int 
token_in(
    const set<enum Kind> *pset,
    const struct parse_state_t *pstate);

int 
token_match(
    const set<enum Kind> *pset,
    struct parse_state_t *pstate);

int 
token_match_one(
    const enum Kind kind,
    struct parse_state_t *pstate);

void 
init_parser(
    struct parse_state_t *pstate,
    vector<struct token_pair_t> *ptoken_pairs);

struct syntax_tree_node_t* 
parse_program(struct parse_state_t *pstate);

void 
parse_declarations(
    SymbolTable *ptable,
    struct parse_state_t *pstate);

struct syntax_tree_node_t* 
parse_stmt_sequence(struct parse_state_t *pstate);

struct syntax_tree_node_t* 
parse_statement(struct parse_state_t *pstate);

struct syntax_tree_node_t* 
parse_if_stmt(struct parse_state_t *pstate);

struct syntax_tree_node_t* 
parse_while_stmt(struct parse_state_t *pstate);

struct syntax_tree_node_t* 
parse_repeat_stmt(struct parse_state_t *pstate);

struct syntax_tree_node_t* 
parse_read_stmt(struct parse_state_t *pstate);

struct syntax_tree_node_t* 
parse_write_stmt(struct parse_state_t *pstate);

struct syntax_tree_node_t* 
parse_assign_stmt(struct parse_state_t *pstate);

struct syntax_tree_node_t* 
parse_bool_exp(struct parse_state_t *pstate);

struct syntax_tree_node_t* 
parse_exp(struct parse_state_t *pstate);

struct syntax_tree_node_t* 
parse_arithmetic_exp(struct parse_state_t *pstate);

struct syntax_tree_node_t* 
parse_or_exp(struct parse_state_t *pstate);

struct syntax_tree_node_t* 
parse_and_exp(struct parse_state_t *pstate);

struct syntax_tree_node_t* 
parse_not_exp(struct parse_state_t *pstate);

struct syntax_tree_node_t* 
parse_factor(struct parse_state_t *pstate);

struct syntax_tree_node_t* 
parse_term(struct parse_state_t *pstate);

struct syntax_tree_node_t* 
parse_compare_exp(struct parse_state_t *pstate);

struct syntax_tree_node_t* 
parse_string_exp(struct parse_state_t *pstate);

#endif
