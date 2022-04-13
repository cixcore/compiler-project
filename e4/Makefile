#
# UFRGS - Compiladores 2021/2
# Grupo H - Carlos Morvan Santiago, Maria Cecília Corrêa
#

etapa4: parser.tab.c lex.yy.c
	g++ -std=c++11 -c lex.yy.c parser.tab.c yylvallib.c tree.c types.cpp -g
	g++ -std=c++11 -o etapa4 main.c lex.yy.o parser.tab.o yylvallib.o tree.o types.o -lfl -g
parser.tab.c: parser.y
	bison -v -d parser.y
lex.yy.c: scanner.l
	flex scanner.l

clean:
	rm -f lex.yy.c parser.tab.c parser.tab.h etapa4 parser.output *.o
