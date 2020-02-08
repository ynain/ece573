team:
	@echo "Team: Bling_Bling_Boyz\n"
	@echo "Yash Nain"
	@echo "ynain\n"
	@echo "Raghav Malik"
	@echo "malik22"

%.o: %.c
	gcc -c $< -o $@ -lfl

lex.yy.c: scanner.lex
	flex scanner.lex

y.tab.h: parser

lexer: y.tab.h lex.yy.o lexer.o
	gcc $^ -o lexer -lfl

y.tab.c: parser.y
	yacc -d parser.y

parser: y.tab.o parseRunner.o lex.yy.o
	gcc $^ -o parser

compiler: lexer parser

clean:
	@rm -f lex.yy.c
	@rm -f y.tab.c
	@rm -f y.tab.h
	@rm -f *.o
	@rm -f lexer
	@rm -f parser

test:
	./runme inputs/*.micro outputs/*.micro.out

diff:
	diff outputs/*.micro.out expected/*.micro.expected
