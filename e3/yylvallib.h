/* Grupo H - Carlos Morvan Santiago, Maria Cecília Corrêa */
#define MAX_CHILDREN 4

#define TRUE 1
#define FALSE 0

#define CE 1
#define OC 2
#define ID 3
#define PR 4
#define LIT_INT   5
#define LIT_FLOAT 6
#define LIT_BOOL  7
#define LIT_CHAR  8
#define LIT_STR   9
#define FUNC_CALL 10

int get_line_number();

union value_token {
  int integer;
  float flt;
  char character;
  char* str;
  int boolean;
};

// Tipo de valor lexico
struct lex_value_t{
    int line;
    int type;
    union value_token token;
};

struct node{
    struct lex_value_t* value;
    struct node* children[MAX_CHILDREN];
    struct node* next;
};

void alloc_lex_value(int type);
void yylval_int(int value);
void yylval_float(float value);
void yylval_char(char value);
void yylval_bool(int value);
void yylval_string(int type, char* value);