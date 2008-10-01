CFLAGS = -Wall -g -O0

try: try.o inkpot_scheme.o inkpot_xlate.o inkpot_value.o

inkpot_scheme.o: inkpot_scheme.c inkpot_scheme.h inkpot.h inkpot_define.h \
	inkpot_value.c inkpot_xlate.c inkpot_scheme_table.h

inkpot_xlate.o: inkpot_xlate.c inkpot_xlate.h inkpot.h

inkpot_value.o: inkpot_value.c inkpot_value.h inkpot.h inkpot_value_table.h

inkpot_value_table.h: inkpot_define.h

inkpot_scheme_table.h: inkpot_define.h

inkpot_define.h: scripts/lib-h.tcl       tk.lib svg.lib x11.lib 
	scripts/lib-h.tcl

x11.lib: scripts/rgb.txt-x11.lib.tcl     data/rgb.txt
	scripts/rgb.txt-x11.lib.tcl data/rgb.txt

svg.lib: scripts/types.html-svg.lib.tcl  data/types.html
	scripts/types.html-svg.lib.tcl data/types.html

tk.lib: scripts/xcolors.c-tk.lib.tcl     data/xcolors.c
	scripts/xcolors.c-tk.lib.tcl data/xcolors.c

clean:
	rm -rf *.o try *.lib inkpot_value_table.h inkpot_scheme_table.h inkpot_define.h
