/* Grupo H - Carlos Morvan Santiago, Maria Cecília Corrêa */
#include "yyvallib.h"
#include <bits/stdc++.h>
using namespace std;

#define INT_T     1
#define FLOAT_T   2
#define CHAR_T    3
#define BOOL_T    4
#define STRING_T  5

#define VAR_N     6
#define LIT_N     7
#define FUNC_N    8
#define VEC_N     9

struct argument {
    char* id;
    int type;
}

struct symtable_content {
    int lin;
    int col;
    int nature;
    int type;
    int size;
    vector<struct argument> arguments;
    union value_token token_value_data;
};