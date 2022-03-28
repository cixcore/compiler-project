/* Grupo H - Carlos Morvan Santiago, Maria Cecília Corrêa */
#include <bits/stdc++.h>
#include "parser.tab.h"
#include "yylvallib.h"
#include "types.h"


list<symbols_table> scopes;


int get_inferred_type(int type1, int type2) {
    if(type1 == type2) return type1;
    if((type1 == INT_T && type2 == FLOAT_T) || (type2 == INT_T && type1 == FLOAT_T)) return FLOAT_T;
    if((type1 == INT_T && type2 == BOOL_T) || (type2 == INT_T && type1 == BOOL_T)) return INT_T;
    if((type1 == FLOAT_T && type2 == BOOL_T) || (type2 == FLOAT_T && type1 == BOOL_T)) return FLOAT_T;

    return -1;
}

const char* int_to_symtable_key(int value) {
    return to_string(value).c_str();
}
const char* float_to_symtable_key(float value) {
    return to_string(value).c_str();
}