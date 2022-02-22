#
# UFRGS - Compiladores 2021/2
# Grupo H - Carlos Morvan Santiago, Maria Cecília Corrêa
#

etapa2: parser.tab.c lex.yy.c
	gcc -c lex.yy.c parser.tab.c
	gcc -o etapa2 main.c lex.yy.o parser.tab.o -lfl
parser.tab.c: parser.y
	bison -v -d parser.y
lex.yy.c: scanner.l
	flex scanner.l

clean:
	rm -f lex.yy.c parser.tab.c parser.tab.h etapa2 parser.output *.o