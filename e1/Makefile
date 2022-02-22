#
# UFRGS - Compiladores 2021/2
# Grupo H - Carlos Morvan Santiago, Maria Cecília Corrêa
#

etapa1: lex.yy.c
	gcc -o etapa1 lex.yy.c -lfl
lex.yy.c: scanner.l
	flex scanner.l

clean:
	rm -f lex.yy.c etapa1