#include <bits/stdc++.h>

#define NU -1 


#define RFP  -2
#define RSP  -3
#define RBSS -4
#define RPC  -5


#define LOADAI  1
#define LOADI   2
#define ADD     3
#define SUB     4
#define MULT    5
#define DIV     6
#define RSUBI   7
#define CMP_LT  8
#define CMP_LE  9
#define CMP_EQ  10
#define CMP_NE  11
#define CMP_GT  12
#define CMP_GE  13
#define CBR     14
#define NOP     15
#define STOREAI 16
#define JUMPI   17
#define JUMP    18
#define I2I     19
#define HALT    20
#define ADDI    21

using label_stack = std::list<std::pair<char*, int>>;


int newRegister();
int newLabel();

void generate_iloc(void* root);
void funcDecCode(struct node* node1, struct lex_value_t* func);
int getFuncLabel(char* func);
void createLabelFunc(struct lex_value_t* func);
struct instr* codeLabel(int label);
struct instr* newInstr(int op, int arg1, int arg2, int arg3);
void setMain(struct lex_value_t* funcName);

void initCode(struct node* node1);
void storeVar(struct node* parent, struct lex_value_t* var, struct node* exp);