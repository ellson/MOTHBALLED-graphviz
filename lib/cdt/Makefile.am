## Process this file with automake to produce Makefile.in

pdfdir = $(pkgdatadir)/doc/pdf
pkgextraincludedir = $(pkgincludedir)
pkgconfigdir = $(pkglibdir)/pkgconfig

man = cdt.3
pdf = cdt.pdf

AM_CPPFLAGS = -I$(top_srcdir)

pkginclude_HEADERS = cdt.h dthdr.h
pkglib_LTLIBRARIES = libcdt.la
man_MANS = $(man)
pdf_DATA = $(pdf)
pkgextrainclude_DATA = $(top_builddir)/ast_common.h
pkgconfig_DATA = libcdt.pc

libcdt_la_SOURCES = dtclose.c dtdisc.c dtextract.c dtflatten.c \
	dthash.c dtlist.c dtmethod.c dtopen.c dtrenew.c dtrestore.c dtsize.c \
	dtstat.c dtstrhash.c dttree.c dtview.c dtwalk.c

.3.pdf:
	groff -Tps -man $< | ps2pdf - - >$@

EXTRA_DIST = $(man) $(pdf) Makefile.old features

DISTCLEANFILES = $(pdf)
