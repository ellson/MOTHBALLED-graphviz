CFLAGS = -Wall -g -O0

test: inkpot_scheme.o inkpot_value.o inkpot_xlate.o test.o

inkpot_scheme.o: inkpot_scheme.c inkpot_scheme.h inkpot.h inkpot_define.h \
	inkpot_value.h  inkpot_xlate.h inkpot_table.h

inkpot_value.o: inkpot_value.c inkpot_value.h inkpot.h inkpot_define.h

inkpot_xlate.o: inkpot_xlate.c inkpot_xlate.h inkpot.h

test.o: test.c inkpot.h



inkpot_scheme.c: ../inkpot_scheme.c
	cp ../inkpot_scheme.c .

inkpot_value.c: ../inkpot_value.c
	cp ../inkpot_value.c .

inkpot_xlate.c: ../inkpot_xlate.c
	cp ../inkpot_xlate.c .

inkpot_scheme.h: ../inkpot_scheme.h
	cp ../inkpot_scheme.h .

inkpot_value.h: ../inkpot_value.h
	cp ../inkpot_value.h .

inkpot_xlate.h: ../inkpot_xlate.h
	cp ../inkpot_xlate.h .

inkpot.h: ../inkpot.h
	cp ../inkpot.h .



clean:
	rm -rf *.o test inkpot_xlate.h inkpot_value.h inkpot_scheme.h \
		inkpot_xlate.c inkpot_value.c inkpot_scheme.c inkpot.h
