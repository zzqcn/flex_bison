all: filter

fb: filter.l filter.y
	bison --defines=filter_syn.h -o filter_syn.cc filter.y
	flex --header-file=./filter_lex.h -o filter_lex.cc filter.l

filter: fb
	g++ -g -O0 -std=c++11 -o $@ -DYYDEBUG=1 filter_lex.cc filter_syn.cc filter.cc

clean:
	rm -f ./filter_syn.h ./filter_syn.cc
	rm -f ./filter_lex.h ./filter_lex.cc
	rm -f ./filter
