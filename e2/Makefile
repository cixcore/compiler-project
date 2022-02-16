etapa2:
	bison -v -d parser.y
	flex scanner.l
	gcc -c lex.yy.c parser.tab.c
	gcc -o etapa2 main.c lex.yy.o parser.tab.o -lfl

clean:
	rm lex.yy.c parser.tab.c parser.tab.h etapa2