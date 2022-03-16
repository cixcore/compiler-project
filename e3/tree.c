/* Grupo H - Carlos Morvan Santiago, Maria Cecília Corrêa */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "tree.h"
#include "parser.tab.h"
#include "yylvallib.h"

void exporta(void *tree)
{
    struct node *treeRoot = (struct node *)tree;
    nodes(treeRoot);
    labels(treeRoot);
}

void nodes(struct node *no)
{
    if (no != NULL)
    {
        for (int i = 0; i < MAX_CHILDREN; i++)
        {
            if (no->children[i])
            {
                printf("%p, %p\n", no, no->children[i]);
            }
        }

        if (no->next != NULL)
        {
            printf("%p, %p\n", no, no->next);
        }

        for (int i = 0; i < MAX_CHILDREN; i++)
        {
            nodes(no->children[i]);
        }

        nodes(no->next);
    }
}

void labels(struct node *no)
{
    if (no != NULL)
    {
        printf("%p [label=\"", no);

        switch (no->value->type)
        {
            case SC:
                printf("%c", no->value->token.character);
                break;
            case OC:
                printf("%s", no->value->token.str);
                break;
            case ID:
                printf("%s", no->value->token.str);
                break;
            case PR:
                printf("%s", no->value->token.str);
                break;
            case LIT_INT:
                printf("%d", no->value->token.integer);
                break;
            case LIT_FLOAT:
                printf("%f", no->value->token.flt);
                break;
            case LIT_BOOL:
                if (no->value->token.boolean == TRUE)
                {
                    printf("true");
                }
                else
                {
                    printf("false");
                }
                break;
            case LIT_CHAR:
                printf("%c", no->value->token.character);
                break;
            case LIT_STR:
                printf("%s", no->value->token.str);
                break;
            case FUNC_CALL:
                printf("call %s", no->value->token.str);
                break;

            default:
                break;
        }
        printf("\"];\n");
        
        for (int i = 0; i < MAX_CHILDREN; i++)
        {
            if (no->children[i] != NULL)
            {
                labels(no->children[i]);
            }
        }

        if (no->next != NULL)
        {
            labels(no->next);
        }

    }
}

void libera(void *tree)
{
    struct node *treeRoot = (struct node *)tree;
    freeTheTreeeees(treeRoot);
}

void freeTheTreeeees(struct node *no)
{
    if (no != NULL)
    {
        for (int i = 0; i< MAX_CHILDREN; i++) {
            freeTheTreeeees(no->children[i]);
        }
        freeTheTreeeees(no->next);

        if(typeStr(no->value->type)) {
            free(no->value->token.str);
        }
        free(no->value);
        free(no);
    }
}

void free_unused_lex_val(struct lex_value_t *lex_val)
{
    if(typeStr(lex_val->type)) {
        free(lex_val->token.str);
    }
    free(lex_val);
}

int typeStr(int type) {
    return type == OC || type == ID || type == PR || type == FUNC_CALL || type == LIT_STR;
}

struct node *connect(struct node *node1, struct node *node2)
{    
    if(node1 != NULL) {
        struct node *iter = node1;
        while (iter != NULL && iter->next != NULL)
        {
            iter = iter->next;
        }
        iter->next = node2;
        return node1;
    }
    return node2;   
}
void createRoot(struct node *node1, struct node *node2)
{
    if (node1 == NULL)
    {
        extern void *arvore;
        arvore = (void *)node2;
    }
    else
    {
        node1->next = node2;
    }
}
struct node *createParentNode1Child(struct node *parent, struct node *child)
{
    parent->children[0] = child;
    parent->children[1] = NULL;
    parent->children[2] = NULL;
    parent->children[3] = NULL;
    return parent;
}
struct node *createParentNode2Children(struct node *parent, struct node *child0, struct node *child1)
{
    parent->children[0] = child0;
    parent->children[1] = child1;
    parent->children[2] = NULL;
    parent->children[3] = NULL;
    return parent;
}
struct node *createParentNode3Children(struct node *parent, struct node *child0, struct node *child1, struct node *child2)
{
    parent->children[0] = child0;
    parent->children[1] = child1;
    parent->children[2] = child2;
    parent->children[3] = NULL;
    return parent;
}
struct node *createParentNode4Children(struct node *parent, struct node *child0, struct node *child1, struct node *child2, struct node *child3)
{
    parent->children[0] = child0;
    parent->children[1] = child1;
    parent->children[2] = child2;
    parent->children[3] = child3;
    return parent;
}

struct node *lexToNode(struct lex_value_t *lex_value)
{
    struct node *newNode = (struct node *)malloc(sizeof(struct node));
    newNode->value = lex_value;
    newNode->next = NULL;
    newNode->children[0] = NULL;
    newNode->children[1] = NULL;
    newNode->children[2] = NULL;
    newNode->children[3] = NULL;
    return newNode;
}

struct node *createLeaf(struct lex_value_t *lex_value)
{
    struct node *newNode = (struct node *)malloc(sizeof(struct node));
    newNode->value = lex_value;
    newNode->next = NULL;
    newNode->children[0] = NULL;
    newNode->children[1] = NULL;
    newNode->children[2] = NULL;
    newNode->children[3] = NULL;
    return newNode;
}
struct node *createNLeaf(struct lex_value_t *lex_value)
{
    struct node *newNode = (struct node *)malloc(sizeof(struct node));
    if (lex_value->type == LIT_INT)
    {
        lex_value->token.integer = -lex_value->token.integer;
    }
    else
    {
        lex_value->token.flt = -lex_value->token.flt;
    }
    newNode->value = lex_value;
    newNode->next = NULL;
    newNode->children[0] = NULL;
    newNode->children[1] = NULL;
    newNode->children[2] = NULL;
    newNode->children[3] = NULL;
    return newNode;
}
struct node *createFuncCallNode(struct lex_value_t *lex_value)
{
    struct node *newNode = (struct node *)malloc(sizeof(struct node));
    lex_value->type = FUNC_CALL;
    newNode->value = lex_value;
    newNode->next = NULL;
    newNode->children[0] = NULL;
    newNode->children[1] = NULL;
    newNode->children[2] = NULL;
    newNode->children[3] = NULL;
    return newNode;
}

struct lex_value_t *lexValueFromSC(char schar)
{
    struct lex_value_t* lexValue = (struct lex_value_t *)malloc(sizeof(struct lex_value_t));
    lexValue->line = get_line_number();
    lexValue->type = SC;
    lexValue->token.character = schar;

    return lexValue;
}
struct lex_value_t *lexValueFromOC(char *schar)
{
    struct lex_value_t* lexValue = (struct lex_value_t *)malloc(sizeof(struct lex_value_t));
    lexValue->line = get_line_number();
    lexValue->type = OC;
    lexValue->token.str = strdup(schar);

    return lexValue;
}
