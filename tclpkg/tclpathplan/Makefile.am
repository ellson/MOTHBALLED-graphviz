## Process this file with automake to produce Makefile.in

SUBDIRS = demo

AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/tclpkg/tclhandle \
	-I$(top_srcdir)/lib/pathplan @TCL_INCLUDES@

AM_CFLAGS = @TCL_CFLAGS@

noinst_HEADERS = simple.h
if WITH_TCL
pkglib_LTLIBRARIES = libtclplan.la
endif

libtclplan_la_SOURCES = find_ints.c intersect.c makecw.c tclpathplan.c wrapper.c

libtclplan_la_LIBADD = \
	$(top_builddir)/tclpkg/tclhandle/libtclhandle.la \
	$(top_builddir)/tclpkg/tclstubs/libtclstubs.la \
	$(top_builddir)/lib/pathplan/libpathplan.la

if WITH_TCL
all-local: pkgIndex.tcl

pkgIndex.tcl: libtclplan.la
	sh $(top_srcdir)/mkpkgindex.sh libtclplan.la Tclpathplan $(VERSION)
endif

CLEANFILES = pkgIndex.tcl

EXTRA_DIST = $(libtclplan_la_SOURCES)
