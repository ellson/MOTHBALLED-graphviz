# $Id$ $Revision$
## Process this file with automake to produce Makefile.in

pdfdir = $(pkgdatadir)/doc/pdf
pkgconfigdir = $(libdir)/pkgconfig

pkginclude_HEADERS = inkpot.h
lib_LTLIBRARIES = libinkpot.la
pkgconfig_DATA = libinkpot.pc

noinst_PROGRAMS = inkpot
noinst_HEADERS = inkpot_scheme.h inkpot_xlate.h inkpot_value.h \
	inkpot_define.h inkpot_value_table.h inkpot_scheme_table.h
noinst_LTLIBRARIES = libinkpot_C.la

man_MANS = inkpot.3
pdf_DATA = inkpot.3.pdf

inkpot_SOURCES = inkpot.c
inkpot_LDADD = $(top_builddir)/lib/inkpot/libinkpot_C.la

libinkpot_C_la_SOURCES = inkpot_scheme.c inkpot_value.c inkpot_xlate.c

libinkpot_la_LDFLAGS = -version-info @VERSION_INFO@ -no-undefined
libinkpot_la_SOURCES = $(libgvc_C_la_SOURCES)

inkpot_value_table.h: inkpot_define.h

inkpot_scheme_table.h: inkpot_define.h

inkpot_define.h: scripts/lib-h.tcl       tk.lib svg.lib x11.lib 
	scripts/lib-h.tcl tk.lib svg.lib x11.lib
	rm tk.lib svg.lib x11.lib

x11.lib: scripts/rgb.txt-x11.lib.tcl     data/rgb.txt
	scripts/rgb.txt-x11.lib.tcl data/rgb.txt

svg.lib: scripts/types.html-svg.lib.tcl  data/types.html
	scripts/types.html-svg.lib.tcl data/types.html

tk.lib: scripts/xcolors.c-tk.lib.tcl     data/xcolors.c
	scripts/xcolors.c-tk.lib.tcl data/xcolors.c

inkpot.3.pdf: $(srcdir)/inkpot.3
	- @GROFF@ -Tps -man $(srcdir)/inkpot.3 | @PS2PDF@ - - > inkpot.3.pdf

EXTRA_DIST = $(man_MANS) $(pdf_DATA) Makefile.old inkpot_define.h \
	inkpot_value_table.h inkpot_scheme_table.h \
	tk.lib svg.lib x11.lib

DISTCLEANFILES = $(pdf_DATA) \
	inkpot_define.h inkpot_value_table.h inkpot_scheme_table.h

