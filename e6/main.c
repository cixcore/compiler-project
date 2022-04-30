#include <stdio.h>
extern int yyparse(void);
extern int yylex_destroy(void);

void *arvore = NULL;
void exporta(void *arvore);
void libera(void *arvore);
void printAssembly(void *arvore);
void initCodeMem(void *arvore);

int main (int argc, char **argv)
{
    int ret = yyparse();
    //exporta(arvore);
    initCodeMem(arvore);
    printAssembly(arvore);
    libera(arvore);
    arvore = NULL;
    yylex_destroy();
    return ret;
}