#
# UFRGS - Compiladores 2021/2
# Grupo H - Carlos Morvan Santiago, Maria Cecília Corrêa
#

etapa3: parser.tab.c lex.yy.c
	gcc -c lex.yy.c parser.tab.c yylvallib.c tree.c -g
	gcc -o etapa3 main.c lex.yy.o parser.tab.o yylvallib.o tree.o -lfl -g
parser.tab.c: parser.y
	bison -v -d parser.y
lex.yy.c: scanner.l
	flex scanner.l

clean:
	rm -f lex.yy.c parser.tab.c parser.tab.h etapa3 parser.output *.o
