CFLAGS = -Wall -g -O0

test: inkpot_scheme.o inkpot_value.o inkpot_xlate.o test.o

inkpot_scheme.o: inkpot_scheme.c inkpot.h inkpot_define.h \
		inkpot_scheme.h inkpot_value.h inkpot_xlate.h \
		inkpot_scheme_table.h

inkpot_value.o: inkpot_value.c inkpot.h inkpot_define.h \
		inkpot_value.h inkpot_value_table.h

inkpot_xlate.o: inkpot_xlate.c inkpot.h inkpot_xlate.h

test.o: test.c inkpot.h



inkpot_scheme.c: ../inkpot_scheme.c
	ln -s ../inkpot_scheme.c

inkpot_value.c: ../inkpot_value.c
	ln -s ../inkpot_value.c

inkpot_xlate.c: ../inkpot_xlate.c
	ln -s ../inkpot_xlate.c

inkpot_scheme.h: ../inkpot_scheme.h
	ln -s ../inkpot_scheme.h

inkpot_value.h: ../inkpot_value.h
	ln -s ../inkpot_value.h

inkpot_xlate.h: ../inkpot_xlate.h
	ln -s ../inkpot_xlate.h

inkpot.h: ../inkpot.h
	ln -s ../inkpot.h



clean:
	rm -rf *.o test inkpot_xlate.h inkpot_value.h inkpot_scheme.h \
		inkpot_xlate.c inkpot_value.c inkpot_scheme.c inkpot.h
