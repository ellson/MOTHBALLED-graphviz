# $Id$ $Revision$
## Process this file with automake to produce Makefile.in

INKPOT_VERSION="1:0:0"

pdfdir = $(pkgdatadir)/doc/pdf
pkgconfigdir = $(libdir)/pkgconfig

pkginclude_HEADERS = inkpot.h
lib_LTLIBRARIES = libinkpot.la
pkgconfig_DATA = libinkpot.pc
bin_PROGRAMS = inkpot

noinst_HEADERS = inkpot_scheme.h inkpot_xlate.h inkpot_value.h \
			inkpot_lib_define.h inkpot_lib_value.h \
			inkpot_lib_scheme.h inkpot_lib_string.h 
noinst_LTLIBRARIES = libinkpot_C.la

man_MANS = inkpot.3 inkpot.1
pdf_DATA = inkpot.3.pdf inkpot.1.pdf

inkpot_SOURCES = inkpot.c
inkpot_LDADD = $(top_builddir)/lib/inkpot/libinkpot.la

libinkpot_C_la_SOURCES = inkpot_scheme.c inkpot_value.c inkpot_xlate.c
libinkpot_C_la_LIBADD = $(top_builddir)/lib/rbtree/librbtree_C.la
libinkpot_C_la_DEPENDENCIES = $(top_builddir)/lib/rbtree/librbtree_C.la

libinkpot_la_SOURCES = $(libinkpot_C_la_SOURCES)
libinkpot_la_LIBADD = $(libinkpot_C_la_LIBADD)
libinkpot_la_LDFLAGS = -version-info $(INKPOT_VERSION) -no-undefined 


inkpot_scheme.o inkpot_scheme.lo: inkpot_lib_define.h \
				inkpot_lib_scheme.h \
				inkpot_lib_string.h

inkpot_value.o inkpot_value.lo: inkpot_lib_define.h \
				inkpot_lib_value.h 

inkpot_lib_value.h: inkpot_lib_define.h

inkpot_lib_scheme.h: inkpot_lib_define.h

inkpot_lib_string.h: inkpot_lib_define.h

inkpot_lib_define.h: inkpot_lib.tcl inkpot_lib_procs.tcl \
			$(top_builddir)/lib/inkpot/inkpot_lib_tk.dat \
       			$(top_builddir)/lib/inkpot/inkpot_lib_svg.dat \
			$(top_builddir)/lib/inkpot/inkpot_lib_x11.dat \
		       	$(top_builddir)/lib/inkpot/inkpot_lib_brewer.dat
	$(TCLSH) inkpot_lib.tcl \
			$(top_builddir)/lib/inkpot/inkpot_lib_tk.dat \
			$(top_builddir)/lib/inkpot/inkpot_lib_svg.dat \
			$(top_builddir)/lib/inkpot/inkpot_lib_x11.dat \
			$(top_builddir)/lib/inkpot/inkpot_lib_brewer.dat

$(top_builddir)/lib/inkpot/inkpot_lib_x11.dat: inkpot_lib_x11.tcl data/rgb.txt
	$(TCLSH) inkpot_lib_x11.tcl data/rgb.txt

$(top_builddir)/lib/inkpot/inkpot_lib_svg.dat: inkpot_lib_svg.tcl data/types.html
	$(TCLSH) inkpot_lib_svg.tcl data/types.html

$(top_builddir)/lib/inkpot/inkpot_lib_tk.dat: inkpot_lib_tk.tcl data/xcolors.c
	$(TCLSH) inkpot_lib_tk.tcl data/xcolors.c

$(top_builddir)/lib/inkpot/inkpot_lib_brewer.dat: inkpot_lib_brewer.tcl data/brewer_colors
	$(TCLSH) inkpot_lib_brewer.tcl data/brewer_colors

inkpot.1.pdf: inkpot.1
	- $(GROFF) -Tps -man inkpot.1 | $(PS2PDF) - - > inkpot.1.pdf

inkpot.3.pdf: inkpot.3
	- $(GROFF) -Tps -man inkpot.3 | $(PS2PDF) - - > inkpot.3.pdf

CLEANFILES = inkpot_lib_*.dat

EXTRA_DIST = $(man_MANS) $(pdf_DATA) *.tcl data/* \
	inkpot_lib_define.h inkpot_lib_value.h inkpot_lib_scheme.h inkpot_lib_string.h

DISTCLEANFILES = $(pdf_DATA) \
	inkpot_lib_define.h inkpot_lib_value.h inkpot_lib_scheme.h inkpot_lib_string.h
