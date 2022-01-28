#
# UFRGS - Compiladores 2021/2
#

etapa1: lex.yy.c
	gcc -o etapa1 lex.yy.c -lfl
lex.yy.c: scanner.l
	flex scanner.l

clean:
	rm lex.yy.c etapa1