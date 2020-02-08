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
	gcc $^ -o lexer

clean:
	rm -f lex.yy.c
	rm -f *.o
	rm -f lexer

test:
	./runme inputs/test1.txt outputs/test1.txt.out
	./runme inputs/test2.txt outputs/test2.txt.out
	./runme inputs/test3.txt outputs/test3.txt.out
	./runme inputs/test4.txt outputs/test4.txt.out
	./runme inputs/test5.txt outputs/test5.txt.out
	./runme inputs/test6.txt outputs/test6.txt.out

diff:
	diff expected/test1.txt.expected outputs/test1.txt.out
	diff expected/test2.txt.expected outputs/test2.txt.out
	diff expected/test3.txt.expected outputs/test3.txt.out
	diff expected/test4.txt.expected outputs/test4.txt.out
	diff expected/test5.txt.expected outputs/test5.txt.out
	diff expected/test6.txt.expected outputs/test6.txt.out
