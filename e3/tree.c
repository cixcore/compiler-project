/* Grupo H - Carlos Morvan Santiago, Maria Cecília Corrêa */
#include <stdlib.h>
#include <string.h> 
#include <stdio.h>
#include "parser.tab.h"
#include "yylvallib.h"


void exporta(void *tree){
    
}
void libera(void *tree){
}

struct node* connect(struct node* node1, struct node* node2){
    struct node* iter = node1->prox;
    while (iter != NULL && iter->prox != NULL)
    {
        iter = iter->prox;
    }
    iter->prox = node2;
    return node1;    
}
void createRoot(struct node* node1, struct node* node2){
    if(node1 == NULL) {
        extern void *arvore;
		arvore = (void*)node2;
    } else {
        node1->next = node2;
    }
}
struct node* createParentNode1Child(struct node* parent, struct node* child){
    *parent.children[0] = child;
    return parent;
}
struct node* createParentNode2Children(struct node* parent, struct node* child0, struct node* child1){
    *parent.children[0] = child0;
    *parent.children[1] = child1;
    return parent;
}
struct node* createParentNode3Children(struct node* parent, struct node* child0, struct node* child1, struct node* child2){
    *parent.children[0] = child0;
    *parent.children[1] = child1;
    *parent.children[2] = child2;
    return parent;
}
struct node* createParentNode4Children(struct node* parent, struct node* child0, struct node* child1, struct node* child2, struct node* child3){
    *parent.children[0] = child0;
    *parent.children[1] = child1;
    *parent.children[2] = child2;
    *parent.children[3] = child3;
    return parent;
}

struct node* lexToNode(struct lex_value_t* lex_value){
}

struct node* createLeaf(struct lex_value_t* lex_value){
    struct node* newNode = (struct node*)malloc(sizeof(struct node));
    node->lexValue = lex_value;
    return newNode;
}
struct node* createNLeaf(struct lex_value_t* lex_value){
    struct node* newNode = (struct node*)malloc(sizeof(struct node));
    if(lex_value->type == LIT_INT) {
        lex_value->token.integer = -lex_value->token.integer;
    } else {
        lex_value->token.flt = -lex_value->token.flt;
    }
    node->lexValue = lex_value;
    return newNode;
}
struct node* createFuncCallLeaf(struct lex_value_t* lex_value){
    struct node* newNode = (struct node*)malloc(sizeof(struct node));
    lex_value->type = FUNC_CALL;
    node->lexValue = lex_value;
    return newNode;
}

struct lex_value_t* lexValueFromSC(char schar){
    struct lexValue* = (struct lex_value_t*)malloc(sizeof(struct lex_value_t));
    lexValue->line = get_line_number();
    lexValue->type = SC;
    lexValue->token.str = schar;

    return lexValue;
}
struct lex_value_t* lexValueFromOC(char* schar){
    struct lexValue* = (struct lex_value_t*)malloc(sizeof(struct lex_value_t));
    lexValue->line = get_line_number();
    lexValue->type = OC;
    lexValue->token.str = strdup(schar);

    return lexValue;
}