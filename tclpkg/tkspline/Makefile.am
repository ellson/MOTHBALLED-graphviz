## Process this file with automake to produce Makefile.in

SUBDIRS = demo

pkgtcldir = $(pkglibdir)/tcl
pdfdir = $(pkgdatadir)/doc/pdf

man = tkspline.n
pdf = tkspline.pdf

AM_CPPFLAGS = \
	-I@TK_SRC_DIR@/generic \
	-I@TCL_SRC_DIR@/generic \
	@TCL_INCLUDES@ @TCLINT_INCLUDES@ \
	@TK_INCLUDES@ @TKINT_INCLUDES@

AM_CFLAGS = @X_CFLAGS@ @TCL_CFLAGS@  @TK_CFLAGS@ @TK_DEFS@ \
	-UPACKAGE_NAME -UPACKAGE_STRING -UPACKAGE_TARNAME -UPACKAGE_VERSION

if WITH_TK
pkgtcl_LTLIBRARIES = libtkspline.la
man_MANS = $(man)
pdf_DATA = $(pdf)
endif

libtkspline_la_SOURCES = tkspline.c
libtkspline_la_LIBADD = \
	$(top_builddir)/tclpkg/tkstubs/libtkstubs.la \
	$(top_builddir)/tclpkg/tclstubs/libtclstubs.la 
libtkspline_la_LDFLAGS = -module -version-info @VERSION_INFO@ -no-undefined

if WITH_TK
all-local: pkgIndex.tcl

pkgIndex.tcl: libtkspline.la
	sh $(top_srcdir)/tclpkg/mkpkgindex.sh libtkspline.la Tkspline $(VERSION)
endif

.n.pdf:
	groff -Tps -man $< | ps2pdf - - >$@

EXTRA_DIST = $(libtkspline_la_SOURCES) $(man) $(pdf) dllEntry.c

CLEANFILES = pkgIndex.tcl

DISTCLEANFILES = $(pdf)
