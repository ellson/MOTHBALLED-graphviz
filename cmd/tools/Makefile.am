# $Id$ $Revision$
## Process this file with automake to produce Makefile.in

CGRAPH_LIBS=$(top_builddir)/lib/cgraph/libcgraph.la
if WITH_CGRAPH
   GRAPH_LIBS=$(CGRAPH_LIBS)
else
   GRAPH_LIBS=$(top_builddir)/lib/graph/libgraph.la
endif

AM_CPPFLAGS = \
        -I$(top_srcdir) \
	-I$(top_srcdir)/lib/cgraph \
	-I$(top_srcdir)/lib/graph \
	-I$(top_srcdir)/lib/cdt \
	-I$(top_srcdir)/lib/pathplan \
	-I$(top_srcdir)/lib/pack \
	-I$(top_srcdir)/lib/neatogen \
	-I$(top_srcdir)/lib/sparse \
	-I$(top_srcdir)/lib/gvc \
	-I$(top_srcdir)/lib/common \
	-I$(top_srcdir)/lib/ingraphs @EXPAT_INCLUDES@

pdfdir = $(pkgdatadir)/doc/pdf

noinst_HEADERS = colortbl.h convert.h mmio.h matrix_market.h
bin_PROGRAMS = gc gvcolor gxl2gv acyclic nop ccomps sccmap tred \
	unflatten gvpack dijkstra bcomps mm2gv
man_MANS = gc.1 gvcolor.1 gxl2gv.1 acyclic.1 nop.1 ccomps.1 sccmap.1 \
	tred.1 unflatten.1 gvpack.1 dijkstra.1 bcomps.1 mm2gv.1
pdf_DATA = gc.pdf gvcolor.pdf gxl2gv.pdf acyclic.pdf nop.pdf ccomps.pdf \
	sccmap.pdf tred.pdf unflatten.pdf gvpack.pdf dijkstra.pdf bcomps.pdf \
	mm2gv.pdf

install-data-hook:
	(cd $(DESTDIR)$(man1dir); rm -f gv2gxl.1; $(LN_S) gxl2gv.1 gv2gxl.1;)
	(cd $(DESTDIR)$(pdfdir); rm -f gv2gxl.pdf; $(LN_S) gxl2gv.pdf gv2gxl.pdf;)

install-exec-hook:
	(cd $(DESTDIR)$(bindir); rm -f gv2gxl$(EXEEXT); $(LN_S) gxl2gv$(EXEEXT) gv2gxl$(EXEEXT);)
	(cd $(DESTDIR)$(bindir); rm -f dot2gxl$(EXEEXT); $(LN_S) gxl2gv$(EXEEXT) dot2gxl$(EXEEXT);)
	(cd $(DESTDIR)$(bindir); rm -f gxl2dot$(EXEEXT); $(LN_S) gxl2gv$(EXEEXT) gxl2dot$(EXEEXT);)

uninstall-hook:
	(cd $(DESTDIR)$(man1dir); rm -f gv2gxl.1;)
	(cd $(DESTDIR)$(pdfdir); rm -f gv2gxl.pdf;)
	(cd $(DESTDIR)$(bindir); rm -f gv2gxl$(EXEEXT);)
	(cd $(DESTDIR)$(bindir); rm -f dot2gxl$(EXEEXT);)
	(cd $(DESTDIR)$(bindir); rm -f gxl2dot$(EXEEXT);)

gxl2gv_SOURCES = cvtgxl.c gv2gxl.c gxl2gv.c

gxl2gv_LDADD = \
	$(top_builddir)/lib/ingraphs/libingraphs_C.la \
	$(CGRAPH_LIBS) @EXPAT_LIBS@ \
	$(GRAPH_LIBS)

sccmap_SOURCES = sccmap.c

sccmap_LDADD = \
	$(top_builddir)/lib/ingraphs/libingraphs_C.la \
	$(CGRAPH_LIBS) @EXPAT_LIBS@ \
	$(GRAPH_LIBS)

ccomps_SOURCES = ccomps.c

ccomps_LDADD = \
	$(top_builddir)/lib/ingraphs/libingraphs_C.la \
	$(GRAPH_LIBS)

tred_SOURCES = tred.c

tred_LDADD = \
	$(top_builddir)/lib/ingraphs/libingraphs_C.la \
	$(GRAPH_LIBS)

unflatten_SOURCES = unflatten.c

unflatten_LDADD = \
	$(top_builddir)/lib/ingraphs/libingraphs_C.la \
	$(CGRAPH_LIBS)

acyclic_SOURCES = acyclic.c

acyclic_LDADD = \
	$(top_builddir)/lib/ingraphs/libingraphs_C.la \
	$(GRAPH_LIBS)

nop_SOURCES = nop.c

nop_LDADD = \
	$(top_builddir)/lib/ingraphs/libingraphs_C.la \
	$(GRAPH_LIBS)

gvcolor_SOURCES = gvcolor.c colxlate.c colortbl.h

gvcolor_LDADD = \
	$(top_builddir)/lib/ingraphs/libingraphs_C.la \
	$(GRAPH_LIBS)

bcomps_SOURCES = bcomps.c

bcomps_LDADD = \
	$(top_builddir)/lib/ingraphs/libingraphs_C.la \
	$(GRAPH_LIBS)

gc_SOURCES = gc.c

gc_LDADD = \
	$(top_builddir)/lib/ingraphs/libingraphs_C.la \
	$(GRAPH_LIBS)

gvpack_SOURCES = gvpack.c gvpack_builtins.c

gvpack_LDADD = \
	$(top_builddir)/lib/gvc/libgvc.la \
	$(top_builddir)/lib/ingraphs/libingraphs_C.la \
	$(top_builddir)/plugin/neato_layout/libgvplugin_neato_layout.la

mm2gv_SOURCES = mm2gv.c matrix_market.c mmio.c

mm2gv_LDADD = \
    $(top_builddir)/lib/common/libcommon_C.la \
	$(top_builddir)/lib/sparse/libsparse_C.la \
	$(GRAPH_LIBS) @MATH_LIBS@

dijkstra_SOURCES = dijkstra.c

dijkstra_LDADD = \
	$(top_builddir)/lib/ingraphs/libingraphs_C.la \
	$(CGRAPH_LIBS)

.1.pdf:
	- @GROFF@ -Tps -man $< | @PS2PDF@ - - >$@

EXTRA_DIST = $(man_MANS) $(pdf_DATA) Makefile.old

CLEANFILES = stamp.h

DISTCLEANFILES = $(pdf_DATA)
