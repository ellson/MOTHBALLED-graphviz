## Process this file with automake to produce Makefile.in

SUBDIRS = demo

pdfdir = $(pkgdatadir)/doc/pdf
htmldir = $(pkgdatadir)/doc/html

AM_CPPFLAGS = \
        -I$(top_srcdir) \
        -I$(top_srcdir)/dotneato/dotgen \
	-I$(top_srcdir)/dotneato/neatogen \
	-I$(top_srcdir)/dotneato/twopigen \
	-I$(top_srcdir)/dotneato/fdpgen \
	-I$(top_srcdir)/dotneato/circogen \
	-I$(top_srcdir)/dotneato/gvrender \
	-I$(top_srcdir)/dotneato/common \
	-I$(top_srcdir)/graph \
	-I$(top_srcdir)/cdt \
	-I$(top_srcdir)/pathplan \
	-I$(top_srcdir)/tclhandle @GD_INCLUDES@ @TCL_INCLUDES@

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

libtcldot_la_SOURCES = tcldot.c tkgen.c $(top_srcdir)/gdtclft/gdtclft.c

libtcldot_la_LIBADD = \
	$(top_builddir)/tclpkg/tclhandle/libtclhandle.la \
	$(top_builddir)/tclpkg/tclstubs/libtclstubs.la \
	$(top_builddir)/dotneato/gvrender/libgvrender.la \
        $(top_builddir)/dotneato/dotgen/libdotgen.la \
        $(top_builddir)/dotneato/neatogen/libneatogen.la \
        $(top_builddir)/dotneato/twopigen/libtwopigen.la \
        $(top_builddir)/dotneato/fdpgen/libfdpgen.la \
        $(top_builddir)/dotneato/circogen/libcircogen.la

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
