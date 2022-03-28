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

using symbols_table = std::map<char*, struct symtable_content*>;
using entry = std::pair<char*, struct symtable_content*>;

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
void id_entry_missing_type(struct lex_value_t *identifier);
void vector_entry_missing_type(struct lex_value_t *identifier, struct lex_value_t *vec_size);

int get_inferred_type(int type1, int type2);
char* string_to_char_array(const char* str);

void clearTypeStructures();
symbols_table deepcopy_symbols_table();
void free_undefs();
void print_scopes();
void print_undef();