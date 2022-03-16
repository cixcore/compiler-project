/*
Função principal para realização da e3.
Não modifique este arquivo.
*/
#include <stdio.h>
extern int yyparse(void);
extern int yylex_destroy(void);

void *arvore = NULL;
void exporta(void *arvore);
void libera(void *arvore);

int main (int argc, char **argv)
{
    int ret = yyparse();
    exporta(arvore);
    printf("finished exporta");
    libera(arvore);
    printf("finished libera");
    arvore = NULL;
    yylex_destroy();
    return ret;
}