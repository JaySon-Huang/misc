#include "tinyp_structs.h"
#include "syntax_tree.h"
#include "stdlib.h"

struct syntax_tree_node_t* new_tree_node(enum NodeType node_type,
                                         struct syntax_tree_node_t* st,
                                         struct syntax_tree_node_t* nd,
                                         struct syntax_tree_node_t* rd) {
    struct syntax_tree_node_t* n =
        (struct syntax_tree_node_t*)malloc(sizeof(struct syntax_tree_node_t));
    n->node_type = node_type;
    n->sibiling = NULL;
    n->child[0] = st;
    n->child[1] = nd;
    n->child[2] = rd;
    return n;
}

void destroyTree(struct syntax_tree_node_t* root) {
    if (root) {
        for (int i = 0; i != 3; ++i) {
            destroyTree(root->child[i]);
        }
        if (root->sibiling) {
            destroyTree(root->sibiling);
        }
        free(root);
        root = NULL;
    }
}

struct syntax_tree_node_t* new_stmt_node(enum NodeType node_type) {
    // struct syntax_tree_node_t *n = (struct
    // syntax_tree_node_t*)malloc(sizeof(struct syntax_tree_node_t));
    struct syntax_tree_node_t* n = new struct syntax_tree_node_t();
    if (n == NULL) {
        printf("Not enough memory.\n");
    } else {
        for (int i = 0; i < 3; ++i) {
            n->child[i] = NULL;
        }
        n->sibiling = NULL;
        n->node_type = node_type;
        n->token.kind = TK_COMMENT;
        n->token.value = "";
    }
    return n;
}

struct syntax_tree_node_t* new_exp_node(enum NodeType node_type) {
    // struct syntax_tree_node_t *n = (struct
    // syntax_tree_node_t*)malloc(sizeof(struct syntax_tree_node_t));
    struct syntax_tree_node_t* n = new struct syntax_tree_node_t();
    if (n == NULL) {
        printf("Not enough memory.\n");
    } else {
        for (int i = 0; i < 3; ++i) {
            n->child[i] = NULL;
        }
        n->sibiling = NULL;
        n->node_type = node_type;
        n->token.kind = TK_COMMENT;
        n->token.value = "";
    }
    return n;
}

static unsigned int indent = 0;

static void print_indent() {
    for (int i = 0; i != indent; ++i) printf(" ");
}

void printTree(struct syntax_tree_node_t* root) {
    if (root == NULL) return;
    indent += 2;
    while (root != NULL) {
        print_indent();
        switch (root->node_type) {
            case STMT_IF:
                printf("If\n");
                break;
            case STMT_REPEAT:
                printf("Repeat\n");
                break;
            case STMT_WHILE:
                printf("While\n");
                break;
            case STMT_ASSIGN:
                printf("Assign to : %s\n", root->token.value.c_str());
                break;
            case STMT_READ:
                printf("Read : %s\n", root->token.value.c_str());
                break;
            case STMT_WRITE:
                printf("Write : %s\n", root->token.value.c_str());
                break;

            case EXP_OP:
                printf("Op: %s\n", root->token.value.c_str());
                break;
            case EXP_CONST:
                printf("Const: %s\n", root->token.value.c_str());
                break;
            case EXP_ID:
                printf("ID : %s\n", root->token.value.c_str());
                break;
            case EXP_STR:
                printf("Str : %s\n", root->token.value.c_str());
                break;

            default:
                printf("Unknown NodeType\n");
                break;
        }
        for (int i = 0; i != 3; ++i) printTree(root->child[i]);
        root = root->sibiling;
    }
    indent -= 2;
}
