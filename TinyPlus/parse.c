#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "tinyp_structs.h"
#include "parse.h"
#include "handlers.h"

void 
_set_errdetail(
    struct parse_state_t* parse_state, 
    const char* reason)
{
    int len = strlen(reason);
    parse_state->errdetail = (char *)malloc(len*sizeof(char)+1);
    strcpy(parse_state->errdetail, reason);
}

int 
init_parser(
    const char* filename,
    struct parse_state_t* parse_state,
    struct token_pair_t** token_pair_head)
{
    parse_state->lineno = 0;
    parse_state->rowno = 0;
    parse_state->errdetail = NULL;
    parse_state->cur_state = STA_START;

    parse_state->fp = fopen(filename, "r");
    if (parse_state->fp == NULL){
        parse_state->errdetail = (char *)malloc(255*sizeof(char));
        sprintf(parse_state->errdetail, "Open file %s failed!", filename);
        return -1;
    }

    *token_pair_head = NULL;
    return 0;
}

int 
destroy_parser(
    struct parse_state_t* parse_state,
    struct token_pair_t** token_pair_head)
{
    struct token_pair_t* to_destroy = NULL;
    if (parse_state->errdetail) {
        free(parse_state->errdetail);
    }
    if (parse_state->fp != NULL) {
        fclose(parse_state->fp);
    }

    while (*token_pair_head != NULL) {
        to_destroy = *token_pair_head;
        *token_pair_head = (*token_pair_head)->next;
        free(to_destroy);
    }
    return 0;
}

struct token_pair_t*
new_node()
{
    struct token_pair_t* n = (struct token_pair_t*)malloc(sizeof(struct token_pair_t*));
    memset(n->value, 0, sizeof(n->value));
    n->value_len = 0;
    return n;
}

int 
push_node(
    struct token_pair_t** token_pair_head, 
    struct token_pair_t* ntp)
{
    // TODO: 加到链表尾
    return 0;
}

int parse(
    struct parse_state_t* parse_state,
    struct token_pair_t** token_pair_head)
{
    int ch;
    int action = ACT_IDLE;
    struct token_pair_t* ntp = new_node();

    while (1){
        ch = get_next_char(parse_state);
        if (DEBUG)
            printf("get char:%c(0x%02X)\n", ch, ch);

        switch(parse_state->cur_state)
        {
        case STA_START:
            action = _handler_start(ch, parse_state, ntp);
            break;
        case STA_ASSIGN:
            action = _handler_assign(ch, parse_state, ntp);
            break;
        case STA_COMMENT:
            action = _handler_comment(ch, parse_state, ntp);
            break;
        case STA_DONE:
            action = _handler_done(ch, parse_state, ntp);
            break;
        case STA_GREATER:
            action = _handler_greater(ch, parse_state, ntp);
            break;
        case STA_LESS:
            action = _handler_less(ch, parse_state, ntp);
            break;
        case STA_ID:
            action = _handler_id(ch, parse_state, ntp);
            break;
        case STA_NUMBER:
            action = _handler_number(ch, parse_state, ntp);
            break;
        case STA_NUMBER_TMP:
            action = _handler_number_tmp(ch, parse_state, ntp);
            break;
        case STA_STRING:
            action = _handler_string(ch, parse_state, ntp);
            break;
        case STA_ERROR:
            action = _handler_error(ch, parse_state, ntp);
            break;

        default:
            fprintf(stderr, "State:%d had not been handled!\n", parse_state->cur_state);
            break;
        }
        
        switch (action)
        {
        case ACT_PUSH_NODE:
            printf("(%d,%s)\n", ntp->kind, ntp->value);
            push_node(token_pair_head, ntp);
            ntp = new_node();
            break;
        case ACT_REPORT_ERROR:
            free(ntp);
            break;
        case ACT_EXIT:
            free(ntp);
            return 0;
        }
    }
    return 0;
}



int get_next_char(
    struct parse_state_t* parse_state)
{
    int ch = 0;
    if ( feof(parse_state->fp) ) {
        return EOF;
    }
    ch = fgetc(parse_state->fp);
    parse_state->rowno += 1;
    if (ch == '\n') {
        parse_state->lineno += 1;
    }
    return ch;
}
