#include "tinyp_structs.h"

void 
token_pair_print(token_pair_t* p)
{
    printf("(");
if (DETAIL){
    switch (p->kind)
    {
    case TK_ADD:
        printf("ADD  ");
        break;
    case TK_AND:
        printf("AND  ");
        break;
    case TK_ASSIGN:
        printf("ASSIGN");
        break;
    case TK_BOOL:
        printf("BOOL ");
        break;
    case TK_COMMA:
        printf("COMMA");
        break;
    case TK_COMMENT:
        printf("COMMENT");
        break;
    case TK_DIV:
        printf("DIV  ");
        break;
    case TK_DO:
        printf("DO   ");
        break;
    case TK_ELSE:
        printf("ELSE ");
        break;
    case TK_END:
        printf("END  ");
        break;
    case TK_ENDFILE:
        printf("ENDFILE");
        break;
    case TK_EQU:
        printf("EQU  ");
        break;
    case TK_FALSE:
        printf("FALSE");
        break;
    case TK_GEQ:
        printf("GEQ  ");
        break;
    case TK_GREATER:
        printf("GREATER");
        break;
    case TK_HEXNUM:
        printf("HEXNUM");
        break;
    case TK_ID:
        printf("ID   ");
        break;
    case TK_IF:
        printf("IF   ");
        break;
    case TK_INT:
        printf("INT  ");
        break;
    case TK_LEQ:
        printf("LEQ  ");
        break;
    case TK_LESS:
        printf("LESS ");
        break;
    case TK_LP:
        printf("LP   ");
        break;
    case TK_MUL:
        printf("MUL  ");
        break;
    case TK_NOT:
        printf("NOT  ");
        break;
    case TK_NUM:
        printf("NUM  ");
        break;
    case TK_OCTNUM:
        printf("OCTNUM");
        break;
    case TK_OR:
        printf("OR  ");
        break;
    case TK_QUOTE:
        printf("QUOTE");
        break;
    case TK_READ:
        printf("READ ");
        break;
    case TK_REPEAT:
        printf("REPEAT");
        break;
    case TK_RP:
        printf("RP   ");
        break;
    case TK_SEMICOLON:
        printf("SEMICOLON");
        break;
    case TK_STR:
        printf("STR  ");
        break;
    case TK_STRING:
        printf("STRING");
        break;
    case TK_SUB:
        printf("SUB  ");
        break;
    case TK_THEN:
        printf("THEN ");
        break;
    case TK_TRUE:
        printf("TRUE ");
        break;
    case TK_UNTIL:
        printf("UNTIL");
        break;
    case TK_WHILE:
        printf("WHILE");
        break;
    case TK_WRITE:
        printf("WRITE");
        break;

    default:
        printf("%d", p->kind);
        break;
    }
}else{
    switch (p->kind)
    {
    case TK_TRUE:  case TK_FALSE:  case TK_OR:
    case TK_AND:   case TK_NOT:    case TK_INT:
    case TK_BOOL:  case TK_STR:    case TK_WHILE:
    case TK_DO:    case TK_IF:     case TK_THEN:
    case TK_ELSE:  case TK_END:    case TK_REPEAT:
    case TK_UNTIL: case TK_READ:   case TK_WRITE:
        printf("KEY");
        break;
    case TK_GREATER: case TK_LEQ:   case TK_GEQ:
    case TK_COMMA:   case TK_QUOTE: case TK_SEMICOLON:
    case TK_ASSIGN:  case TK_ADD:   case TK_SUB:
    case TK_MUL:     case TK_DIV:   case TK_LP:
    case TK_RP:      case TK_LESS:  case TK_EQU:
        printf("SYM"); 
        break;
    case TK_STRING:
        printf("STR");
        break;
    case TK_COMMENT:
        printf("COM");
        break;
    case TK_ENDFILE:
        printf("EOF");
        break;
    case TK_ID:
        printf("ID ");
        break;
    case TK_NUM:case TK_OCTNUM:case TK_HEXNUM:
        printf("NUM");
        break;

    default:
        printf("%d", p->kind);
        break;
    }
}
    printf(", %s)", p->value.c_str());
}

/**
 *   关键字:
 *   TK_TRUE, TK_FALSE, TK_OR,
 *   TK_AND, TK_NOT, TK_INT,
 *   TK_BOOL, TK_STRING, TK_WHILE,
 *   TK_DO, TK_IF, TK_THEN,
 *   TK_ELSE, TK_END, TK_REPEAT,
 *   TK_UNTIL, TK_READ, TK_WRITE,
 */
void 
token_pair_kind_key(token_pair_t* p)
{
    if (p->value == "true"){
        p->kind = TK_TRUE;
    }else if (p->value == "false"){
        p->kind = TK_FALSE;
    }else if (p->value == "or"){
        p->kind = TK_OR;
    }else if (p->value == "and"){
        p->kind = TK_AND;
    }else if (p->value == "not"){
        p->kind = TK_NOT;
    }else if (p->value == "int"){
        p->kind = TK_INT;
    }else if (p->value == "bool"){
        p->kind = TK_BOOL;
    }else if (p->value == "string"){
        p->kind = TK_STR;
    }else if (p->value == "while"){
        p->kind = TK_WHILE;
    }else if (p->value == "do"){
        p->kind = TK_DO;
    }else if (p->value == "if"){
        p->kind = TK_IF;
    }else if (p->value == "then"){
        p->kind = TK_THEN;
    }else if (p->value == "else"){
        p->kind = TK_ELSE;
    }else if (p->value == "end"){
        p->kind = TK_END;
    }else if (p->value == "repeat"){
        p->kind = TK_REPEAT;
    }else if (p->value == "until"){
        p->kind = TK_UNTIL;
    }else if (p->value == "read"){
        p->kind = TK_READ;
    }else if (p->value == "write"){
        p->kind = TK_WRITE;
    }else {
        p->kind = TK_ID;
    }
    
}

void
token_pair_copy(
    token_pair_t *dest,
    const token_pair_t *src)
{
    dest->kind = src->kind;
    dest->value.assign(src->value);
}
