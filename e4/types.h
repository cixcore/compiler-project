/* Grupo H - Carlos Morvan Santiago, Maria Cecília Corrêa */
#include <bits/stdc++.h>
#define INT_T     1
#define FLOAT_T   2
#define CHAR_T    3
#define BOOL_T    4
#define STRING_T  5

#define VAR_N     6
#define LIT_N     7
#define FUNC_N    8
#define VEC_N     9

#define INT_SIZE_BYTES      4
#define FLOAT_SIZE_BYTES    8
#define CHAR_SIZE_BYTES     1
#define BOOL_SIZE_BYTES     1

using symbols_table = std::map<const char*, struct symtable_content*>;
using entry = std::pair<const char*, struct symtable_content*>;

struct argument {
    char* id;
    int type;
};

struct symtable_content {
    int lin;
    int col;
    int nature;
    int type;
    int size;
    std::vector<struct argument> arguments;
    union value_token token_value_data;
};

void init_types_and_add_to_scope(int type);
int bytes_of(int type);
void create_entry_missing_type(struct lex_value_t *identifier);
void create_entry_vector_size_missing_type(struct lex_value_t *identifier, struct lex_value_t *vec_size);

int get_inferred_type(int type1, int type2);
const char* int_to_symtable_key(int value);
const char* float_to_symtable_key(float value);

void clearTypeStructures();