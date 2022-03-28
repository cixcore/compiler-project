/* Grupo H - Carlos Morvan Santiago, Maria Cecília Corrêa */
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

using namespace std;
using symbols_table = map<const char*, struct symtable_content>;

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
    vector<struct argument> arguments;
    union value_token token_value_data;
};

int get_inferred_type(int type1, int type2);
const char* int_to_symtable_key(int value);
const char* float_to_symtable_key(float value);