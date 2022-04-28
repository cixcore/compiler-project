/* Grupo H - Carlos Morvan Santiago, Maria Cecília Corrêa */
#include <bits/stdc++.h>
#define MAX_CHILDREN 4


struct instr{
    int label;
    int op;
    int arg1;
    int arg2;
    int arg3;
    int notation;
    struct instr* next;
};


struct node{
    int type;
    struct lex_value_t* value;
    struct node* children[MAX_CHILDREN];
    struct node* next;

    struct instr* code;
    struct instr* codeEnd;
    int temp;
    std::list<int*> patchTrue;
    std::list<int*> patchFalse;
};


void exporta(void *tree);
void libera(void *tree);
void labels(struct node *no);
void nodes(struct node *no);
void freeTheTreeeees(struct node *no);
void free_lex_val(struct lex_value_t *lex_val);
int typeStr(int type);
struct node* connect(struct node* node1, struct node* node2);
void createRoot(struct node* node1, struct node* node2);
struct node* createParentNode1Child(struct node* parent, struct node* child);
struct node* createParentNode2Children(struct node* parent, struct node* child1, struct node* child2);
struct node* createParentNode3Children(struct node* parent, struct node* child1, struct node* child2, struct node* child3);
struct node* createParentNode4Children(struct node* parent, struct node* child1, struct node* child2, struct node* child3, struct node* child4);

struct node* lexToNode(struct lex_value_t* lex_value);

struct node* createLeaf(struct lex_value_t* lex_value);
struct node* createNLeaf(struct lex_value_t* lex_value);
struct node* createFuncCallNode(struct lex_value_t* lex_value);

struct lex_value_t* lexValueFromSC(char schar);
struct lex_value_t* lexValueFromOC(const char* schar);


void codeConcat(struct node* root);
void codeConcatCommands(struct node* node1, struct node* node1next);
