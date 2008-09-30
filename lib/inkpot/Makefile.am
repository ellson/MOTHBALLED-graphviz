CFLAGS = -Wall -g -O0

try: try.o inkpot_scheme.o inkpot_xlate.o inkpot_value.o

inkpot_scheme.o: inkpot_scheme.c inkpot_scheme.h inkpot.h inkpot_define.h \
	inkpot_value.c inkpot_xlate.c inkpot_scheme_table.h

inkpot_xlate.o: inkpot_xlate.c inkpot_xlate.h inkpot.h

inkpot_value.o: inkpot_value.c inkpot_value.h inkpot.h inkpot_value_table.h

inkpot_value_table.h: inkpot_define.h

inkpot_scheme_table.h: inkpot_define.h

inkpot_define.h: x11.lib scripts/lib_2_h.tcl
	scripts/lib_2_h.tcl

x11.lib: data/rgb.txt scripts/rgb_txt_2_x11_lib.tcl
	scripts/rgb_txt_2_x11_lib.tcl data/rgb.txt

svg.lib: data/types.html scripts/types_html_t_svg_lib.tcl
	scripts/types_html_t_svg_lib.tcl data/types.html

clean:
	rm -rf *.o try *.lib inkpot_table.h inkpot_define.h
