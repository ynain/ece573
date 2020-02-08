team:
	@echo "Team: Bling_Bling_Boyz\n"
	@echo "Yash Nain"
	@echo "ynain\n"
	@echo "Raghav Malik"
	@echo "malik22"

%.o: %.c
	gcc -c $< -o $@

lex.yy.c: scanner.lex
	flex scanner.lex

compiler: lex.yy.o lexer.o
	# gcc $^ -o lexer
	bison parser.y -d -o parser.cpp
	g++ $^ -o lexer

clean:
	rm -f lex.yy.c
	rm -f *.o
	rm -f parser.cpp parser.tab.hpp
	rm -f lexer

test:
	./runme inputs/*.txt outputs/*.txt.out

diff:
	diff expected/*.txt.expected outputs/*.txt.out
