# $Id$ $Revision$
## Process this file with automake to produce Makefile.in

SUBDIRS = demo

pdfdir = $(pkgdatadir)/doc/pdf
htmldir = $(pkgdatadir)/doc/html

pkgtcldir = $(pkglibdir)/tcl

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
pkgtcl_LTLIBRARIES = libtcldot.la libtcldot_builtin.la
man_MANS = $(man)
pdf_DATA = $(pdf)
html_DATA = $(html)
endif

if WITH_CODEGENS
CODEGENS = tkgen.c
endif

if WITH_LIBGD
GDTCLFT = $(top_srcdir)/tclpkg/gdtclft/gdtclft.c
endif

libtcldot_la_SOURCES = tcldot.c $(CODEGENS) $(GDTCLFT)
libtcldot_la_LDFLAGS = -no-undefined
libtcldot_la_LIBADD = \
	$(top_builddir)/tclpkg/tclhandle/libtclhandle_C.la \
	$(top_builddir)/tclpkg/tclstubs/libtclstubs_C.la \
	$(top_builddir)/lib/gvc/libgvc.la @LIBGEN_LIBS@ @LIBLTDL@

libtcldot_builtin_la_SOURCES = \
	tcldot.c $(CODEGENS) $(GDTCLFT) \
	$(top_srcdir)/lib/gvc/dot_builtins.c \
	$(top_srcdir)/lib/gvc/no_demand_loading.c 
libtcldot_builtin_la_LDFLAGS = -no-undefined
if WITH_LIBGD
if WITH_MYLIBGD
libtcldot_builtin_la_LIBADD = \
	$(top_builddir)/tclpkg/tclhandle/libtclhandle_C.la \
	$(top_builddir)/tclpkg/tclstubs/libtclstubs_C.la \
	$(top_builddir)/plugin/dot_layout/libgvplugin_dot_layout_C.la \
	$(top_builddir)/plugin/neato_layout/libgvplugin_neato_layout_C.la \
	$(top_builddir)/plugin/core/libgvplugin_core_C.la \
	$(top_builddir)/plugin/gd/libgvplugin_gd_C.la \
	$(top_builddir)/plugin/pango/libgvplugin_pango_C.la \
	$(top_builddir)/lib/gvc/libgvc_C.la \
	$(top_builddir)/lib/graph/libgraph_C.la \
	$(top_builddir)/lib/cdt/libcdt_C.la \
	$(top_builddir)/lib/pathplan/libpathplan_C.la \
	$(top_builddir)/lib/pack/libpack_C.la \
	$(top_builddir)/lib/gd/libgvgd_C.la \
	@PANGOCAIRO_LIBS@ @ICONV_LIBS@ @FC_LIBS@ @FT_LIBS@ @JPEG_LIBS@ @PNG_LIBS@ @EXPAT_LIBS@ @Z_LIBS@ @LIBGEN_LIBS@ @LIBLTDL@ @SOCKET_LIBS@ @IPSEPCOLA_LIBS@
else
libtcldot_builtin_la_LIBADD = \
	$(top_builddir)/tclpkg/tclhandle/libtclhandle_C.la \
	$(top_builddir)/tclpkg/tclstubs/libtclstubs_C.la \
	$(top_builddir)/plugin/dot_layout/libgvplugin_dot_layout_C.la \
	$(top_builddir)/plugin/neato_layout/libgvplugin_neato_layout_C.la \
	$(top_builddir)/plugin/core/libgvplugin_core_C.la \
	$(top_builddir)/plugin/gd/libgvplugin_gd_C.la \
	$(top_builddir)/plugin/pango/libgvplugin_pango_C.la \
	$(top_builddir)/lib/gvc/libgvc_C.la \
	$(top_builddir)/lib/graph/libgraph_C.la \
	$(top_builddir)/lib/cdt/libcdt_C.la \
	$(top_builddir)/lib/pathplan/libpathplan_C.la \
	$(top_builddir)/lib/pack/libpack_C.la \
	@PANGOCAIRO_LIBS@ @GD_LIBS@ @ICONV_LIBS@ @FC_LIBS@ @FT_LIBS@ @JPEG_LIBS@ @PNG_LIBS@ @EXPAT_LIBS@ @Z_LIBS@ @LIBGEN_LIBS@ @LIBLTDL@ @SOCKET_LIBS@ @IPSEPCOLA_LIBS@
endif
else
libtcldot_builtin_la_LIBADD = \
	$(top_builddir)/tclpkg/tclhandle/libtclhandle_C.la \
	$(top_builddir)/tclpkg/tclstubs/libtclstubs_C.la \
	$(top_builddir)/plugin/dot_layout/libgvplugin_dot_layout_C.la \
	$(top_builddir)/plugin/neato_layout/libgvplugin_neato_layout_C.la \
	$(top_builddir)/plugin/core/libgvplugin_core_C.la \
	$(top_builddir)/plugin/pango/libgvplugin_pango_C.la \
	$(top_builddir)/lib/gvc/libgvc_C.la \
	$(top_builddir)/lib/graph/libgraph_C.la \
	$(top_builddir)/lib/cdt/libcdt_C.la \
	$(top_builddir)/lib/pathplan/libpathplan_C.la \
	$(top_builddir)/lib/pack/libpack_C.la \
	@PANGOCAIRO_LIBS@ @ICONV_LIBS@ @EXPAT_LIBS@ @Z_LIBS@ @LIBGEN_LIBS@ @LIBLTDL@ @SOCKET_LIBS@ @IPSEPCOLA_LIBS@
endif

if WITH_TCL
all-local: pkgIndex.tcl

pkgIndex.tcl: libtcldot.la
	sh $(top_srcdir)/tclpkg/mkpkgindex.sh libtcldot.la Tcldot $(VERSION)
endif

.n.pdf:
	groff -Tps -man $< | ps2pdf - - >$@

EXTRA_DIST = $(man) $(pdf) $(html) README README.Tkspline Makefile.old

CLEANFILES = pkgIndex.tcl

DISTCLEANFILES = $(pdf)
