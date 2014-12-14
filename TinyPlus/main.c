#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "tinyp_structs.h"
#include "parse.h"


int main(int argc, char const *argv[])
{
    struct parse_state_t parse_state;
    struct token_pair_t* token_pair_head;
    init_parser("example.tinyp", &parse_state, &token_pair_head);



    destroy_parser(&parse_state, &token_pair_head);
    return 0;
}
