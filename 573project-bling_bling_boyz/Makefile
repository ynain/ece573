COMPILE=g++ -std=c++0x -g -w -Iheaders/ -Iout/

team:
	@echo "Team: Bling_Bling_Boyz\n"
	@echo "Yash Nain"
	@echo "ynain\n"
	@echo "Raghav Malik"
	@echo "malik22"

out/%.o: source/%.c
	${COMPILE} -c $< -o $@

out/%.o: source/%.cpp
	${COMPILE} -c $< -o $@

out/%.o: out/%.cpp
	${COMPILE} -c $< -o $@

out/%.o: out/%.c
	${COMPILE} -c $< -o $@

out/lex.yy.c: source/scanner.lex
	flex $^
	mv lex.yy.c out/lex.yy.c

parser.tab.hpp: parser

lexer: out/parser.tab.hpp out/lex.yy.o lexer.o
	${COMPILE} $^ -o lexer

out/parser.tab.cpp: source/parser.ypp
	bison --warnings=none -d source/parser.ypp
	mv parser.tab.* out/

symbolTable.o: types.h
codeGeneration.o: asts.h

parser: out/parser.tab.o out/parseRunner.o out/lex.yy.o

symbols: parser out/symbolTable.o

loops: symbols out/loops.o

# intermediate: symbols out/codeGeneration.o out/constructors.o out/liveness.o out/registers.o
intermediate: loops out/codeGeneration.o out/constructors.o out/liveness.o out/registers.o
	${COMPILE} out/*.o -o compiler

compiler: intermediate

test: compiler
	@echo "Testing $(testcase)"
	@mkdir -p mine
	@rm -f mine/*
	@./runme phase3-tests/input/test$(testcase).micro mine/test$(testcase).out > /dev/null
	@diff -bB mine/test$(testcase).out phase3-tests/output/test$(testcase).out
	

clean:
	@rm -f compiler
	@rm -f *.tiny
	@rm -f out/*

oldclean:
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
	@rm -f intermediate
	@rm -f *.exe
	@rm -f *.tiny
	@rm -f out/*
