#ifndef __TINYP_SYNTAX_TREE_H__
#define __TINYP_SYNTAX_TREE_H__

#include "symboltable.h"
#include "tinyp_structs.h"

struct syntax_tree_node_t* new_tree_node(enum NodeType node_type,
                                         struct syntax_tree_node_t* st,
                                         struct syntax_tree_node_t* nd,
                                         struct syntax_tree_node_t* rd);

void parse_state_next_token(struct parse_state_t* pstate);

struct syntax_tree_node_t* new_stmt_node(enum NodeType node_type);

struct syntax_tree_node_t* new_exp_node(enum NodeType node_type);

void printTree(struct syntax_tree_node_t* root);

void destroyTree(struct syntax_tree_node_t* root);

#endif
