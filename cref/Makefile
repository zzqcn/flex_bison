all: origin cref

origin: fb2-5.l
	flex -o fb2-5.c fb2-5.l
	gcc -o fb2-5 fb2-5.c

cref: cref.l cref.h cref.cc
	flex --header-file=./cref_lex.h -o cref_lex.cc cref.l
	g++ -g -O0 -o cref cref_lex.cc cref.cc

clean:
	rm -f ./cref
	rm -f ./cref_lex.h ./cref_lex.cc
	rm -f ./fb2-5
	rm -f ./fb2-5.c
