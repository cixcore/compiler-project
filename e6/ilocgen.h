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

#define RET_FUNC 1
#define MAIN_VAR 2
#define UPDATE_RSP 3
#define RET_LOAD 4
#define CALL 5
#define FUNC_LABEL 6
#define FUNC_CALL 7

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

void initCode(struct node* var_list);
void loadVar(struct node* no, char* id);
void loadLit(struct node* no);
void storeVar(struct node* parent, struct lex_value_t* var, struct node* exp);
void funcCallCode(struct node* node1);
void returnCode(struct node* node1);
void ifCode(struct node* node1);
void codeFor(struct node* node1);
void codeWhile(struct node* node1);
void codeOr(struct node* parent);
void codeAnd(struct node* parent);
void codeRelOp(struct node* parent, int op);
void codeBinaryOp(struct node* parent, int op);
void codeUnaryOp(struct node* parent, struct node* exp);
void codeTernaryOp(struct node* parent);

void initCodeMem(void *tree);
void printCode(void *tree);

void printReg(int id);
void printRegConstRegInstr(char* op, struct instr* instr1);
void printStoreAI(struct instr* instr1);
void printOneReg(char* op, struct instr* instr1);
void printTwoReg(char* op, struct instr* instr1);
void printThreeReg(char* op, struct instr* instr1, char arrow);

void patch(std::list<int*> patchList, int label);
std::list<int*> concatPatches(std::list<int*> patchList1, std::list<int*> patchList2);
