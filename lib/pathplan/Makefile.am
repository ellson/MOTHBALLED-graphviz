## Process this file with automake to produce Makefile.in

pdfdir = $(pkgdatadir)/doc/pdf
pkgconfigdir = $(libdir)/pkgconfig

man = pathplan.3
pdf = pathplan.pdf

pkginclude_HEADERS = pathgeom.h pathplan.h pathutil.h solvers.h \
	tri.h vis.h vispath.h
pkglib_LTLIBRARIES = libpathplan.la
pkgconfig_DATA = libpathplan.pc
man_MANS = $(man)
pdf_DATA = $(pdf)

libpathplan_la_LDFLAGS = -version-info @VERSION_INFO@ -no-undefined

libpathplan_la_SOURCES = cvt.c inpoly.c route.c shortest.c \
	shortestpth.c solvers.c triang.c util.c visibility.c

libpathplan_la_LIBADD = @MATH_LIBS@

.3.pdf:
	groff -Tps -man $< | ps2pdf - - >$@

EXTRA_DIST = $(man) $(pdf) Makefile.old path.lefty

DISTCLEANFILES = $(pdf)
