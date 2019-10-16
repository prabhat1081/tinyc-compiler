tinyc: ass6_13CS10060_target_translator.o ass6_13CS10060_translator.o ass6_13CS10060.tab.o lex.yy.o libprintl.a
	g++ -g -std=c++11 ass6_13CS10060_target_translator.o ass6_13CS10060_translator.o ass6_13CS10060.tab.o lex.yy.o -o tinyc -lfl
	# Usage ./tinyc filename -d(y/n) -s(number)
	# use option -d to output debug files
	# eg ./tinyc filename -d
	# Use -s to specify file number and print according to assignment


ass6_13CS10060_target_translator.o: ass6_13CS10060_target_translator.cxx
	g++ -g -std=c++11   -c ass6_13CS10060_target_translator.cxx

ass6_13CS10060_translator.o: ass6_13CS10060_translator.cxx
	g++ -g -std=c++11   -c ass6_13CS10060_translator.cxx

ass6_13CS10060.tab.o: ass6_13CS10060.tab.c
	g++ -g -std=c++11  -c ass6_13CS10060.tab.c

ass6_13CS10060.tab.h: ass6_13CS10060.y
	bison -dtv  ass6_13CS10060.y

ass6_13CS10060.tab.c: ass6_13CS10060.y
	bison -dtv  ass6_13CS10060.y

lex.yy.o: lex.yy.c ass6_13CS10060.tab.h
	g++ -g -std=c++11  -c lex.yy.c

lex.yy.c: ass6_13CS10060.l
	flex ass6_13CS10060.l

libprintl.a: ass2_13CS10060.o
	ar -rcs libprintl.a ass2_13CS10060.o

ass2_13CS10060.o: ass2_13CS10060.c myl.h
	gcc -g -Wall -c ass2_13CS10060.c

clean:
	rm tinyc ass2_13CS10060.o libprintl.a ass6_13CS10060.tab.c lex.yy.c lex.yy.o ass6_13CS10060.tab.h ass6_13CS10060.tab.o ass6_13CS10060_translator.o  ass6_13CS10060_target_translator.o ass6_13CS10060.output

run1: tinyc ass6_13CS10060_test1.c
	./tinyc ass6_13CS10060_test1.c -dn -s01


run2: tinyc ass6_13CS10060_test2.c
	./tinyc ass6_13CS10060_test2.c -dn -s02


run3: tinyc ass6_13CS10060_test3.c
	./tinyc ass6_13CS10060_test3.c -dn -s03


run4: tinyc ass6_13CS10060_test4.c
	./tinyc ass6_13CS10060_test4.c -dn -s04


run5: tinyc ass6_13CS10060_test5.c
	./tinyc ass6_13CS10060_test5.c -dn -s05


run6: tinyc ass6_13CS10060_test6.c
	./tinyc ass6_13CS10060_test6.c -dn -s06


run7: tinyc ass6_13CS10060_test7.c
	./tinyc ass6_13CS10060_test7.c -dn -s07

run8: tinyc ass6_13CS10060_test8.c
	./tinyc ass6_13CS10060_test8.c -dn -s08

run9: tinyc ass6_13CS10060_test9.c
	./tinyc ass6_13CS10060_test9.c -dn -s09

run10: tinyc ass6_13CS10060_test10.c
	./tinyc ass6_13CS10060_test10.c -dn -s10

run11: tinyc ass6_13CS10060_test11.c
	./tinyc ass6_13CS10060_test11.c -dn -s11

run12: tinyc ass6_13CS10060_test12.c
	./tinyc ass6_13CS10060_test12.c -dn -s12

run13: tinyc ass6_13CS10060_test13.c
	./tinyc ass6_13CS10060_test13.c -dn -s13

run14: tinyc ass6_13CS10060_test14.c
	./tinyc ass6_13CS10060_test14.c -dn -s14

run15: tinyc ass6_13CS10060_test15.c
	./tinyc ass6_13CS10060_test15.c -dn -s15

run16: tinyc ass6_13CS10060_test16.c
	./tinyc ass6_13CS10060_test16.c -dn -s16

run17: tinyc ass6_13CS10060_test17.c
	./tinyc ass6_13CS10060_test17.c -dn -s17

run18: tinyc ass6_13CS10060_test18.c
	./tinyc ass6_13CS10060_test18.c -dn -s18

run19: tinyc ass6_13CS10060_test19.c
	./tinyc ass6_13CS10060_test19.c -dn -s19

runall: tinyc ass6_13CS10060_test1.c ass6_13CS10060_test2.c ass6_13CS10060_test3.c ass6_13CS10060_test4.c ass6_13CS10060_test5.c ass6_13CS10060_test6.c ass6_13CS10060_test7.c ass6_13CS10060_test8.c ass6_13CS10060_test9.c ass6_13CS10060_test10.c ass6_13CS10060_test11.c  ass6_13CS10060_test12.c ass6_13CS10060_test13.c ass6_13CS10060_test14.c ass6_13CS10060_test15.c ass6_13CS10060_test16.c ass6_13CS10060_test17.c ass6_13CS10060_test18.c ass6_13CS10060_test19.c 
	./tinyc ass6_13CS10060_test1.c -dn -s01
	./tinyc ass6_13CS10060_test2.c -dn -s02
	./tinyc ass6_13CS10060_test3.c -dn -s03
	./tinyc ass6_13CS10060_test4.c -dn -s04
	./tinyc ass6_13CS10060_test5.c -dn -s05
	./tinyc ass6_13CS10060_test6.c -dn -s06
	./tinyc ass6_13CS10060_test7.c -dn -s07
	./tinyc ass6_13CS10060_test8.c -dn -s08
	./tinyc ass6_13CS10060_test9.c -dn -s09
	./tinyc ass6_13CS10060_test10.c -dn -s10
	./tinyc ass6_13CS10060_test12.c -dn -s12
	./tinyc ass6_13CS10060_test13.c -dn -s13
	./tinyc ass6_13CS10060_test14.c -dn -s14
	./tinyc ass6_13CS10060_test15.c -dn -s15
	./tinyc ass6_13CS10060_test16.c -dn -s16
	./tinyc ass6_13CS10060_test17.c -dn -s17
	./tinyc ass6_13CS10060_test18.c -dn -s18
	./tinyc ass6_13CS10060_test19.c -dn -s19



cleantests:
	rm *.asm *.out *.o *.sym *.s *temp.c