CFLAGS = -Wall -g -O0

all: test inkpot_strings.h

test: inkpot_scheme.o inkpot_value.o inkpot_xlate.o test.o

inkpot_scheme.o: inkpot_scheme.c inkpot.h \
	inkpot_define.h inkpot_value.h inkpot_struct.h  \
	inkpot_name_table.h inkpot_value_table.h \
	inkpot_xlate.h

inkpot_xlate.o: inkpot_xlate.c inkpot.h inkpot_xlate.h

inkpot_value.o: inkpot_value.c inkpot.h inkpot_value.h inkpot_value_table.h

test.o: test.c inkpot.h

inkpot_strings.h: x11.lib scripts/lib_2_h.tcl
	scripts/lib_2_h.tcl

x11.lib: data/rgb.txt scripts/rgb_txt_2_x11_lib.tcl
	scripts/rgb_txt_2_x11_lib.tcl data/rgb.txt

clean:
	rm -rf *.o test *.lib inkpot_strings.h
