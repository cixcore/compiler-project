/* Grupo H - Carlos Morvan Santiago, Maria Cecília Corrêa */
#define MAX_CHILDREN 4


struct node{
    struct lex_value_t* value;
    struct node* children[MAX_CHILDREN];
    struct node* next;
};


void exporta(void *tree);
void libera(void *tree);