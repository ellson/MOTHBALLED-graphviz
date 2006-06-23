# $Id$ $Revision$
## Process this file with cmake to produce Makefile

INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_SOURCE_DIR}
)


########### next target ###############

SET(gc_SRCS
	gc.c
)

ADD_EXECUTABLE(gc ${gc_SRCS})

#TARGET_LINK_LIBRARIES(gc  ${QT_AND_KDECORE_LIBS} )

INSTALL_TARGETS(/bin gc )


########### next target ###############

SET(gvcolor_SRCS
	gvcolor.c
	colxlate.c
	colortbl.h
)

ADD_EXECUTABLE(gvcolor ${gvcolor_SRCS})

#TARGET_LINK_LIBRARIES(gvcolor  ${QT_AND_KDECORE_LIBS} )

INSTALL_TARGETS(/bin gvcolor )


########### next target ###############

SET(gxl2dot_SRCS
	cvtgxl.c
	dot2gxl.c
	gxl2dot.c
)

ADD_EXECUTABLE(gxl2dot ${gxl2dot_SRCS})

#TARGET_LINK_LIBRARIES(gxl2dot  ${QT_AND_KDECORE_LIBS} )

INSTALL_TARGETS(/bin gxl2dot )


########### next target ###############

SET(acyclic_SRCS
	acyclic.c
)

ADD_EXECUTABLE(acyclic ${acyclic_SRCS})

#TARGET_LINK_LIBRARIES(acyclic  ${QT_AND_KDECORE_LIBS} )

INSTALL_TARGETS(/bin acyclic )


########### next target ###############

SET(nop_SRCS
	nop.c
)

ADD_EXECUTABLE(nop ${nop_SRCS})

#TARGET_LINK_LIBRARIES(nop  ${QT_AND_KDECORE_LIBS} )

INSTALL_TARGETS(/bin nop )


########### next target ###############

SET(ccomps_SRCS
	ccomps.c
)

ADD_EXECUTABLE(ccomps ${ccomps_SRCS})

#TARGET_LINK_LIBRARIES(ccomps  ${QT_AND_KDECORE_LIBS} )

INSTALL_TARGETS(/bin ccomps )


########### next target ###############

SET(sccmap_SRCS
	sccmap.c
)

ADD_EXECUTABLE(sccmap ${sccmap_SRCS})

#TARGET_LINK_LIBRARIES(sccmap  ${QT_AND_KDECORE_LIBS} )

INSTALL_TARGETS(/bin sccmap )


########### next target ###############

SET(tred_SRCS
	tred.c
)

ADD_EXECUTABLE(tred ${tred_SRCS})

#TARGET_LINK_LIBRARIES(tred  ${QT_AND_KDECORE_LIBS} )

INSTALL_TARGETS(/bin tred )


########### next target ###############

SET(unflatten_SRCS
	unflatten.c
)

ADD_EXECUTABLE(unflatten ${unflatten_SRCS})

#TARGET_LINK_LIBRARIES(unflatten  ${QT_AND_KDECORE_LIBS} )

INSTALL_TARGETS(/bin unflatten )


########### next target ###############

SET(gvpack_SRCS
	gvpack.c
	gvpack_builtins.c
)

ADD_EXECUTABLE(gvpack ${gvpack_SRCS})

#TARGET_LINK_LIBRARIES(gvpack  ${QT_AND_KDECORE_LIBS} )

INSTALL_TARGETS(/bin gvpack )


########### next target ###############

SET(dijkstra_SRCS
	dijkstra.c
)

ADD_EXECUTABLE(dijkstra ${dijkstra_SRCS})

#TARGET_LINK_LIBRARIES(dijkstra  ${QT_AND_KDECORE_LIBS} )

INSTALL_TARGETS(/bin dijkstra )


########### next target ###############

SET(bcomps_SRCS
	bcomps.c
)

ADD_EXECUTABLE(bcomps ${bcomps_SRCS})

#TARGET_LINK_LIBRARIES(bcomps  ${QT_AND_KDECORE_LIBS} )

INSTALL_TARGETS(/bin bcomps )


########### install files ###############

INSTALL_FILES( FILES
	gc.pdf
	gvcolor.pdf
	gxl2dot.pdf
	acyclic.pdf
	nop.pdf
	ccomps.pdf
	sccmap.pdf
	tred.pdf
	unflatten.pdf
	gvpack.pdf
	dijkstra.pdf
	bcomps.pdf
)



#original Makefile.am contents follow:

## $Id$ $Revision$
### Process this file with automake to produce Makefile.in
#
#AM_CPPFLAGS = \
#        -I$(top_srcdir) \
#        -I$(top_srcdir)/lib/graph \
#	-I$(top_srcdir)/lib/agraph \
#	-I$(top_srcdir)/lib/cdt \
#	-I$(top_srcdir)/lib/pathplan \
#	-I$(top_srcdir)/lib/pack \
#	-I$(top_srcdir)/lib/neatogen \
#	-I$(top_srcdir)/lib/gvc \
#	-I$(top_srcdir)/lib/common \
#	-I$(top_srcdir)/lib/ingraphs @EXPAT_INCLUDES@
#
#pdfdir = $(pkgdatadir)/doc/pdf
#
#noinst_HEADERS = colortbl.h convert.h
#bin_PROGRAMS = gc gvcolor gxl2dot acyclic nop ccomps sccmap tred \
#	unflatten gvpack dijkstra bcomps
#man_MANS = gc.1 gvcolor.1 gxl2dot.1 acyclic.1 nop.1 ccomps.1 sccmap.1 \
#	tred.1 unflatten.1 gvpack.1 dijkstra.1 bcomps.1
#pdf_DATA = gc.pdf gvcolor.pdf gxl2dot.pdf acyclic.pdf nop.pdf ccomps.pdf \
#	sccmap.pdf tred.pdf unflatten.pdf gvpack.pdf dijkstra.pdf bcomps.pdf
#
#install-data-hook:
#	(cd $(DESTDIR)$(man1dir); rm -f dot2gxl.1; $(LN_S) gxl2dot.1 dot2gxl.1;)
#	(cd $(DESTDIR)$(pdfdir); rm -f dot2gxl.pdf; $(LN_S) gxl2dot.pdf dot2gxl.pdf;)
#
#install-exec-hook:
#	(cd $(DESTDIR)$(bindir); rm -f dot2gxl; $(LN_S) gxl2dot dot2gxl;)
#
#uninstall-hook:
#	(cd $(DESTDIR)$(man1dir); rm -f dot2gxl.1;)
#	(cd $(DESTDIR)$(pdfdir); rm -f dot2gxl.pdf;)
#	(cd $(DESTDIR)$(bindir); rm -f dot2gxl;)
#
#gxl2dot_SOURCES = cvtgxl.c dot2gxl.c gxl2dot.c
#
#gxl2dot_LDADD = \
#	$(top_builddir)/lib/ingraphs/libingraphs_C.la \
#	$(top_builddir)/lib/agraph/libagraph.la @EXPAT_LIBS@ \
#	$(top_builddir)/lib/graph/libgraph.la
#
#sccmap_SOURCES = sccmap.c
#
#sccmap_LDADD = \
#	$(top_builddir)/lib/ingraphs/libingraphs_C.la \
#	$(top_builddir)/lib/agraph/libagraph.la
#
#ccomps_SOURCES = ccomps.c
#
#ccomps_LDADD = \
#	$(top_builddir)/lib/ingraphs/libingraphs_C.la \
#	$(top_builddir)/lib/graph/libgraph.la
#
#tred_SOURCES = tred.c
#
#tred_LDADD = \
#	$(top_builddir)/lib/ingraphs/libingraphs_C.la \
#	$(top_builddir)/lib/graph/libgraph.la
#
#unflatten_SOURCES = unflatten.c
#
#unflatten_LDADD = \
#	$(top_builddir)/lib/ingraphs/libingraphs_C.la \
#	$(top_builddir)/lib/agraph/libagraph.la
#
#acyclic_SOURCES = acyclic.c
#
#acyclic_LDADD = \
#	$(top_builddir)/lib/ingraphs/libingraphs_C.la \
#	$(top_builddir)/lib/graph/libgraph.la
#
#nop_SOURCES = nop.c
#
#nop_LDADD = \
#	$(top_builddir)/lib/ingraphs/libingraphs_C.la \
#	$(top_builddir)/lib/agraph/libagraph.la
#
#gvcolor_SOURCES = gvcolor.c colxlate.c colortbl.h
#
#gvcolor_LDADD = \
#	$(top_builddir)/lib/ingraphs/libingraphs_C.la \
#	$(top_builddir)/lib/graph/libgraph.la
#
#bcomps_SOURCES = bcomps.c
#
#bcomps_LDADD = \
#	$(top_builddir)/lib/ingraphs/libingraphs_C.la \
#	$(top_builddir)/lib/graph/libgraph.la
#
#gc_SOURCES = gc.c
#
#gc_LDADD = \
#	$(top_builddir)/lib/ingraphs/libingraphs_C.la \
#	$(top_builddir)/lib/graph/libgraph.la
#
#gvpack_SOURCES = gvpack.c gvpack_builtins.c
#
#gvpack_LDADD = \
#	$(top_builddir)/lib/gvc/libgvc.la \
#	$(top_builddir)/lib/ingraphs/libingraphs_C.la \
#	$(top_builddir)/lib/plugin/libgvplugin_neato_layout.la
#
#dijkstra_SOURCES = dijkstra.c
#
#dijkstra_LDADD = \
#	$(top_builddir)/lib/ingraphs/libingraphs_C.la \
#	$(top_builddir)/lib/agraph/libagraph.la
#
#.1.pdf:
#	groff -Tps -man $< | ps2pdf - - >$@
#
#EXTRA_DIST = $(man_MANS) $(pdf_DATA) Makefile.old
#
#CLEANFILES = stamp.h
#
#DISTCLEANFILES = $(pdf_DATA)
