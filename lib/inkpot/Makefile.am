# $Id$ $Revision$
## Process this file with automake to produce Makefile.in

pdfdir = $(pkgdatadir)/doc/pdf
pkgconfigdir = $(libdir)/pkgconfig

pkginclude_HEADERS = inkpot.h
lib_LTLIBRARIES = libinkpot.la
pkgconfig_DATA = libinkpot.pc
bin_PROGRAMS = inkpot

noinst_HEADERS = inkpot_scheme.h inkpot_xlate.h inkpot_value.h \
	inkpot_define.h inkpot_value_table.h inkpot_scheme_table.h
noinst_LTLIBRARIES = libinkpot_C.la

man_MANS = inkpot.3 inkpot.1
pdf_DATA = inkpot.3.pdf inkpot.1.pdf

inkpot_SOURCES = inkpot.c
inkpot_LDADD = $(builddir)/libinkpot.la

libinkpot_la_SOURCES = inkpot_scheme.c inkpot_value.c inkpot_xlate.c
libinkpot_la_LDFLAGS = -version-info $(VERSION_INFO) -no-undefined

libinkpot_C_la_SOURCES = $(libgvc_la_SOURCES)

inkpot_value_table.h: $(builddir)/inkpot_define.h

inkpot_scheme_table.h: $(builddir)/inkpot_define.h

inkpot_define.h: scripts/lib-h.tcl \
			$(builddir)/tk.lib \
       			$(builddir)/svg.lib \
			$(builddir)/x11.lib \
		       	$(builddir)/brewer.lib
	$(TCLSH) scripts/lib-h.tcl tk.lib svg.lib x11.lib brewer.lib

x11.lib: scripts/rgb.txt-x11.lib.tcl data/rgb.txt
	$(TCLSH) scripts/rgb.txt-x11.lib.tcl data/rgb.txt

svg.lib: scripts/types.html-svg.lib.tcl data/types.html
	$(TCLSH) scripts/types.html-svg.lib.tcl data/types.html

tk.lib: scripts/xcolors.c-tk.lib.tcl data/xcolors.c
	$(TCLSH) scripts/xcolors.c-tk.lib.tcl data/xcolors.c

brewer.lib: scripts/brewer_colors-brewer.lib.tcl data/brewer_colors
	$(TCLSH) scripts/brewer_colors-brewer.lib.tcl data/brewer_colors

inkpot.1.pdf: inkpot.1
	- $(GROFF) -Tps -man inkpot.1 | $(PS2PDF) - - > inkpot.1.pdf

inkpot.3.pdf: inkpot.3
	- $(GROFF) -Tps -man inkpot.3 | $(PS2PDF) - - > inkpot.3.pdf

CLEANFILES = tk.lib svg.lib x11.lib  brewer.lib \
	inkpot_define.h inkpot_value_table.h inkpot_scheme_table.h

EXTRA_DIST = $(man_MANS) $(pdf_DATA) Makefile.old scripts/* data/*

DISTCLEANFILES = $(pdf_DATA)
