#include "tinyp_structs.h"
#include "syntax_tree.h"
#include "stdlib.h"

struct syntax_tree_node_t*
new_tree_node(
    enum NodeType node_type,
    struct syntax_tree_node_t* st,
    struct syntax_tree_node_t* nd,
    struct syntax_tree_node_t* rd)
{
    struct syntax_tree_node_t *n = (struct syntax_tree_node_t*)malloc(sizeof(struct syntax_tree_node_t));
    n->node_type = node_type;
    n->child[0] = st;
    n->child[1] = nd;
    n->child[2] = rd;
    return n;
}

