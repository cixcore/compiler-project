/* Grupo H - Carlos Morvan Santiago, Maria Cecília Corrêa */
#include <stdlib.h>
#include <stdio.h>
#include "parser.tab.h"
#include "yylvallib.h"

extern int get_line_number();

void alloc_lex_value(int type) {
    yylval.valor_lexico = (struct lex_value_t*)malloc(sizeof(struct lex_value_t));
    yylval.valor_lexico->line = get_line_number();
    yylval.valor_lexico->type  = type;
}

void yylval_int(int value) {
    alloc_lex_value(LIT_INT);
    yylval.valor_lexico->token.integer = value;
}

void yylval_float(float value) {
    alloc_lex_value(LIT_FLOAT);
    yylval.valor_lexico->token.flt = value;
}

void yylval_char(char value) {
    alloc_lex_value(LIT_CHAR);
    yylval.valor_lexico->token.character = value;
}

void yylval_char_sc(char value) {
    alloc_lex_value(SC);
    yylval.valor_lexico->token.character = value;
}

void yylval_bool(int value) {
    alloc_lex_value(LIT_BOOL);
    yylval.valor_lexico->token.boolean = value;
}

void yylval_string(int type, char* value) {
    alloc_lex_value(type);
    yylval.valor_lexico->token.str = value;
}