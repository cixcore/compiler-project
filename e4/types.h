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

struct cmp_char_array {
   bool operator()(char const *a, char const *b) const {
      return std::strcmp(a, b) < 0;
   }
};
using symbols_table = std::map<char*, struct symtable_content*, cmp_char_array>;
using entry = std::pair<char*, struct symtable_content*>;

struct argument {
    char* id;
    int type;
    int col;
    int lin;
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
void create_entry_with_args(struct lex_value_t *identifier, int type, int nat, int mult);
void collect_arg(struct lex_value_t *identifier, int type);
void create_entry(struct lex_value_t *identifier, int type, int nat, int mult);
void id_entry_missing_type(struct lex_value_t *identifier);
void vector_entry_missing_type(struct lex_value_t *identifier, struct lex_value_t *vec_size);
void create_entry_missing_type(struct lex_value_t *identifier, int nat, int mult);

void validate_err_declared_symbol(symtable_content content, char* symtable_key, int nature);

void pop_scope();
void push_scope();

int bytes_of(int type);
int get_inferred_type(int type1, int type2);
char* string_to_char_array(const char* str);

void clearTypeStructures();
symbols_table deepcopy_symbols_table();
void free_symbols_table(symbols_table table);
void free_undefs();
void print_scopes();
void print_undef();
void print_type(int type);
char* nature_str(int nature);