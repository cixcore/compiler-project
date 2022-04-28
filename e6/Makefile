#
# UFRGS - Compiladores 2021/2
# Grupo H - Carlos Morvan Santiago, Maria Cecília Corrêa
#

etapa5: parser.tab.c lex.yy.c
	g++ -std=c++11 -c lex.yy.c parser.tab.c yylvallib.c tree.cpp types.cpp ilocgen.cpp -g
	g++ -std=c++11 -o etapa5 main.c lex.yy.o parser.tab.o yylvallib.o tree.o types.o ilocgen.o -lfl -g
parser.tab.c: parser.y
	bison -v -d parser.y
lex.yy.c: scanner.l
	flex scanner.l

clean:
	rm -f lex.yy.c parser.tab.c parser.tab.h etapa5 parser.output *.o
