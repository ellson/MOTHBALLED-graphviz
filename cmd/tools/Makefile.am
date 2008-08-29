# $Id$ $Revision$
## Process this file with automake to produce Makefile.in

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

noinst_HEADERS = colortbl.h convert.h mmio.h matrix_market.h graph_generator.h
bin_PROGRAMS = gc gvcolor gxl2gv acyclic nop ccomps sccmap tred \
	unflatten gvpack dijkstra bcomps mm2gv gvgen
man_MANS = gc.1 gvcolor.1 gxl2gv.1 acyclic.1 nop.1 ccomps.1 sccmap.1 \
	tred.1 unflatten.1 gvpack.1 dijkstra.1 bcomps.1 mm2gv.1 gvgen.1
pdf_DATA = gc.1.pdf gvcolor.1.pdf gxl2gv.1.pdf acyclic.1.pdf \
	   nop.1.pdf ccomps.1.pdf sccmap.1.pdf tred.1.pdf \
	   unflatten.1.pdf gvpack.1.pdf dijkstra.1.pdf \
	   bcomps.1.pdf mm2gv.1.pdf gvgen.1.pdf

install-data-hook:
	(cd $(DESTDIR)$(man1dir); rm -f gv2gxl.1; $(LN_S) gxl2gv.1 gv2gxl.1;)
	(cd $(DESTDIR)$(pdfdir); rm -f gv2gxl.1.pdf; $(LN_S) gxl2gv.1.pdf gv2gxl.1.pdf;)

install-exec-hook:
	(cd $(DESTDIR)$(bindir); rm -f gv2gxl$(EXEEXT); $(LN_S) gxl2gv$(EXEEXT) gv2gxl$(EXEEXT);)
	(cd $(DESTDIR)$(bindir); rm -f dot2gxl$(EXEEXT); $(LN_S) gxl2gv$(EXEEXT) dot2gxl$(EXEEXT);)
	(cd $(DESTDIR)$(bindir); rm -f gxl2dot$(EXEEXT); $(LN_S) gxl2gv$(EXEEXT) gxl2dot$(EXEEXT);)

uninstall-hook:
	(cd $(DESTDIR)$(man1dir); rm -f gv2gxl.1;)
	(cd $(DESTDIR)$(pdfdir); rm -f gv2gxl.1.pdf;)
	(cd $(DESTDIR)$(bindir); rm -f gv2gxl$(EXEEXT);)
	(cd $(DESTDIR)$(bindir); rm -f dot2gxl$(EXEEXT);)
	(cd $(DESTDIR)$(bindir); rm -f gxl2dot$(EXEEXT);)

gxl2gv_SOURCES = cvtgxl.c gv2gxl.c gxl2gv.c

gxl2gv_LDADD = \
	$(top_builddir)/lib/ingraphs/libingraphs_C.la \
	$(top_builddir)/lib/cgraph/libcgraph.la @EXPAT_LIBS@ 

gxl2gv.1.pdf: $(srcdir)/gxl2gv.1
	- @GROFF@ -Tps -man $(srcdir)/gxl2gv.1 | @PS2PDF@ - - >gxl2gv.1.pdf

sccmap_SOURCES = sccmap.c

sccmap_LDADD = \
	$(top_builddir)/lib/ingraphs/libingraphs_C.la \
	$(top_builddir)/lib/cgraph/libcgraph.la @EXPAT_LIBS@ 

sccmap.1.pdf: $(srcdir)/sccmap.1
	- @GROFF@ -Tps -man $(srcdir)/sccmap.1 | @PS2PDF@ - - >sccmap.1.pdf

ccomps_SOURCES = ccomps.c

ccomps_LDADD = \
	$(top_builddir)/lib/ingraphs/libingraphs_C.la \
	$(top_builddir)/lib/cgraph/libcgraph.la

ccomps.1.pdf: $(srcdir)/ccomps.1
	- @GROFF@ -Tps -man $(srcdir)/ccomps.1 | @PS2PDF@ - - >ccomps.1.pdf

tred_SOURCES = tred.c

tred_LDADD = \
	$(top_builddir)/lib/ingraphs/libingraphs_C.la \
	$(top_builddir)/lib/cgraph/libcgraph.la

tred.1.pdf: $(srcdir)/tred.1
	- @GROFF@ -Tps -man $(srcdir)/tred.1 | @PS2PDF@ - - >tred.1.pdf

unflatten_SOURCES = unflatten.c

unflatten_LDADD = \
	$(top_builddir)/lib/ingraphs/libingraphs_C.la \
	$(top_builddir)/lib/cgraph/libcgraph.la

unflatten.1.pdf: $(srcdir)/unflatten.1
	- @GROFF@ -Tps -man $(srcdir)/unflatten.1 | @PS2PDF@ - - >unflatten.1.pdf

acyclic_SOURCES = acyclic.c

acyclic_LDADD = \
	$(top_builddir)/lib/ingraphs/libingraphs_C.la \
	$(top_builddir)/lib/cgraph/libcgraph.la

acyclic.1.pdf: $(srcdir)/acyclic.1
	- @GROFF@ -Tps -man $(srcdir)/acyclic.1 | @PS2PDF@ - - >acyclic.1.pdf

nop_SOURCES = nop.c

nop_LDADD = \
	$(top_builddir)/lib/ingraphs/libingraphs_C.la \
	$(top_builddir)/lib/cgraph/libcgraph.la

nop.1.pdf: $(srcdir)/nop.1
	- @GROFF@ -Tps -man $(srcdir)/nop.1 | @PS2PDF@ - - >nop.1.pdf

gvcolor_SOURCES = gvcolor.c colxlate.c colortbl.h

gvcolor_LDADD = \
	$(top_builddir)/lib/ingraphs/libingraphs_C.la \
	$(top_builddir)/lib/cgraph/libcgraph.la

gvcolor.1.pdf: $(srcdir)/gvcolor.1
	- @GROFF@ -Tps -man $(srcdir)/gvcolor.1 | @PS2PDF@ - - >gvcolor.1.pdf

bcomps_SOURCES = bcomps.c

bcomps_LDADD = \
	$(top_builddir)/lib/ingraphs/libingraphs_C.la \
	$(top_builddir)/lib/cgraph/libcgraph.la

bcomps.1.pdf: $(srcdir)/bcomps.1
	- @GROFF@ -Tps -man $(srcdir)/bcomps.1 | @PS2PDF@ - - >bcomps.1.pdf

gc_SOURCES = gc.c

gc_LDADD = \
	$(top_builddir)/lib/ingraphs/libingraphs_C.la \
	$(top_builddir)/lib/cgraph/libcgraph.la

gc.1.pdf: $(srcdir)/gc.1
	- @GROFF@ -Tps -man $(srcdir)/gc.1 | @PS2PDF@ - - >gc.1.pdf

gvpack_SOURCES = gvpack.c gvpack_builtins.c

gvpack_LDADD = \
	$(top_builddir)/lib/gvc/libgvc.la \
	$(top_builddir)/lib/ingraphs/libingraphs_C.la \
	$(top_builddir)/plugin/neato_layout/libgvplugin_neato_layout.la

gvpack.1.pdf: $(srcdir)/gvpack.1
	- @GROFF@ -Tps -man $(srcdir)/gvpack.1 | @PS2PDF@ - - >gvpack.1.pdf

mm2gv_SOURCES = mm2gv.c matrix_market.c mmio.c

mm2gv_LDADD = \
    $(top_builddir)/lib/common/libcommon_C.la \
	$(top_builddir)/lib/sparse/libsparse_C.la \
	$(top_builddir)/lib/cgraph/libcgraph.la @MATH_LIBS@

mm2gv.1.pdf: mm2gv.1
	- @GROFF@ -Tps -man mm2gv.1 | @PS2PDF@ - - >mm2gv.1.pdf

dijkstra_SOURCES = dijkstra.c

dijkstra_LDADD = \
	$(top_builddir)/lib/ingraphs/libingraphs_C.la \
	$(top_builddir)/lib/cgraph/libcgraph.la

dijkstra.1.pdf: $(srcdir)/dijkstra.1
	- @GROFF@ -Tps -man $(srcdir)/dijkstra.1 | @PS2PDF@ - - >dijkstra.1.pdf

gvgen_SOURCES = gvgen.c graph_generator.c

gvgen_LDADD = \
	$(top_builddir)/lib/cgraph/libcgraph.la @MATH_LIBS@

gvgen.1.pdf: $(srcdir)/gvgen.1
	- @GROFF@ -Tps -man $(srcdir)/gvgen.1 | @PS2PDF@ - - >gvgen.1.pdf

EXTRA_DIST = $(man_MANS) $(pdf_DATA) Makefile.old

CLEANFILES = stamp.h

DISTCLEANFILES = $(pdf_DATA)
