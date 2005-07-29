## Process this file with automake to produce Makefile.in

SUBDIRS = demo

pdfdir = $(pkgdatadir)/doc/pdf
htmldir = $(pkgdatadir)/doc/html

AM_CPPFLAGS = \
        -I$(top_srcdir) \
	-I$(top_srcdir)/lib/gvc \
	-I$(top_srcdir)/lib/common \
	-I$(top_srcdir)/lib/graph \
	-I$(top_srcdir)/lib/cdt \
	-I$(top_srcdir)/lib/pathplan \
	-I$(top_srcdir)/tclpkg/tclhandle @GD_INCLUDES@ @TCL_INCLUDES@

LIBS =  @LIBS@ -lc

AM_CFLAGS = @TCL_CFLAGS@

man = tcldot.n
pdf = tcldot.pdf
html = tcldot.html

noinst_HEADERS = tcldot.h
if WITH_TCL
pkglib_LTLIBRARIES = libtcldot.la
man_MANS = $(man)
pdf_DATA = $(pdf)
html_DATA = $(html)
endif

libtcldot_la_SOURCES = tcldot.c tkgen.c \
	$(top_srcdir)/tclpkg/gdtclft/gdtclft.c \
	$(top_srcdir)/lib/gvc/builtins.c
libtcldot_la_LIBADD = \
	$(top_builddir)/tclpkg/tclhandle/libtclhandle.la \
	$(top_builddir)/tclpkg/tclstubs/libtclstubs.la \
        $(top_builddir)/lib/plugin/libgvplugin_dot_layout.la \
        $(top_builddir)/lib/plugin/libgvplugin_neato_layout.la \
        $(top_builddir)/lib/plugin/libgvplugin_usershape_gd.la \
	$(top_builddir)/lib/gvc/libgvc.la

if WITH_TCL
all-local: pkgIndex.tcl

pkgIndex.tcl: libtcldot.la
	sh $(top_srcdir)/tclpkg/mkpkgindex.sh libtcldot.la Tcldot $(VERSION)
endif

.n.pdf:
	groff -Tps -man $< | ps2pdf - - >$@

EXTRA_DIST = $(libtcldot_la_SOURCES) $(man) $(pdf) $(html) \
	README README.Tkspline Makefile.old

CLEANFILES = pkgIndex.tcl

DISTCLEANFILES = $(pdf)
