CFLAGS = -Wall -g -O0

LIBINKPOT = inkpot_scheme.o inkpot_xlate.o inkpot_value.o

inkpot: inkpot.o $(LIBINKPOT)

inkpot.o: inkpot.c inkpot.h

inkpot_scheme.o: inkpot_scheme.c inkpot_scheme.h inkpot.h \
	inkpot_lib_define.h inkpot_lib_scheme.h \
	inkpot_value.h inkpot_xlate.h

inkpot_xlate.o: inkpot_xlate.c inkpot_xlate.h inkpot.h

inkpot_value.o: inkpot_value.c inkpot_value.h inkpot.h \
	inkpot_lib_define.h inkpot_value_table.h

inkpot.c:
	ln -s ../inkpot.c
inkpot.h:
	ln -s ../inkpot.h
inkpot_scheme.c:
	ln -s ../inkpot_scheme.c
inkpot_scheme.h:
	ln -s ../inkpot_scheme.h
inkpot_value.c:
	ln -s ../inkpot_value.c
inkpot_value.h:
	ln -s ../inkpot_value.h
inkpot_xlate.c:
	ln -s ../inkpot_xlate.c
inkpot_xlate.h:
	ln -s ../inkpot_xlate.h

clean:
	rm -rf *.o inkpot inkpot.[ch] inkpot_scheme.[ch] \
		inkpot_value.[ch] inkpot_xlate.[ch]



