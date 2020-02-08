team:
	@echo "Team: Bling_Bling_Boyz\n"
	@echo "Yash Nain"
	@echo "ynain\n"
	@echo "Raghav Malik"
	@echo "malik22"

%.o: %.c
	g++ -c $< -o $@ -g

%.o: %.cpp
	g++ -c $< -o $@ -g -std=c++11

lex.yy.c: scanner.lex
	flex scanner.lex

parser.tab.hpp: parser

lexer: parser.tab.hpp lex.yy.o lexer.o
	g++ $^ -o lexer -g

parser.tab.cpp: parser.ypp
	bison -d parser.ypp

parser: parser.tab.o parseRunner.o lex.yy.o
	g++ $^ -o parser -g

symbols: parser.tab.o parseRunner.o lex.yy.o symbolTable.o
	g++ $^ -o symbols -g

# compiler: parser symbols
compiler: symbols
test: compiler
	@echo "Testing $(testcase)"
	@mkdir -p mine
	@rm -f mine/*
	@./runme phase2-tests/input/test$(testcase).micro mine/test$(testcase).out > /dev/null
	@diff -b mine/test$(testcase).out phase2-tests/output/test$(testcase).out


clean:
	@rm -f lex.yy.c
	@rm -f lex.yy.cc
	@rm -f y.tab.c
	@rm -f parser.tab.cpp
	@rm -f y.tab.h
	@rm -f parser.tab.hpp
	@rm -f *.o
	@rm -f lexer
	@rm -f parser
	@rm -f symbols
