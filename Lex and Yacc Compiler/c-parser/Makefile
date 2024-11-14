calc: lex.yy.c y.tab.c
	gcc -g lex.yy.c y.tab.c -o c-parser

lex.yy.c: y.tab.c c-lexer.l
	flex c-lexer.l

y.tab.c: c-parser.y
	bison -y -d c-parser.y

clean: 
	rm -f lex.yy.c y.tab.c y.tab.h c-parser

