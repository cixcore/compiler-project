/* Grupo H - Carlos Morvan Santiago, Maria Cecília Corrêa */
#include <bits/stdc++.h>
#define UND_T     -1
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

void local_init_types_validate_and_add_to_scope(int type);
void global_init_types_and_add_to_scope(int type);
void create_func_entry_with_args(struct lex_value_t *identifier, int type, int nat);
void collect_arg(struct lex_value_t *identifier, int type);
void create_entry(struct lex_value_t *identifier, int type, int nat, int mult);
void declare_id_entry_missing_type(struct lex_value_t *identifier);
void declare_vector_entry_missing_type(struct lex_value_t *identifier, struct lex_value_t *vec_size);
void declare_entry_missing_type(struct lex_value_t *identifier, int nat, int mult);
void declare_id_entry_missing_type_init_id(struct lex_value_t *id_to_add, struct lex_value_t *id_init);
void declare_id_entry_missing_type_init_lit(struct lex_value_t *id_to_add, struct node *lit_init);

void validate_err_declared_symbol(symtable_content content, char* symtable_key, int nature);
int get_type_or_err_undeclared_symbol(struct lex_value_t id_init, int nature);
void validate_not_string_vector(int type, int nature, symtable_content content, char* symtable_key);
void validate_nature(int expected_n, int actual_n, int line);
void validate_err_function_string(int type, int line, int col);

void pop_scope();
void push_scope();

int bytes_of(int type);
int size_of(struct lex_value_t id_init);
int get_inferred_type_validate_char_string(int type1, int type2, symtable_content content);
char* string_to_char_array(const char* str);

void clearTypeStructures();
symbols_table deepcopy_symbols_table();
void free_symbols_table(symbols_table table);
void free_undefs();
void print_scopes();
void print_undef();
char* type_str(int type);
char* nature_str(int nature);