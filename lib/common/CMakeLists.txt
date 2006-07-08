# $Id$ $Revision$
## Process this file with cmake to produce Makefile

INCLUDE_DIRECTORIES(
	${CMAKE_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}/lib/gvc
	${CMAKE_SOURCE_DIR}/lib/pack
	${CMAKE_SOURCE_DIR}/lib/fdpgen
	${CMAKE_SOURCE_DIR}/lib/pathplan
	${CMAKE_SOURCE_DIR}/lib/graph
	${CMAKE_SOURCE_DIR}/lib/cdt
	${CMAKE_SOURCE_DIR}/lib/gd
)

########### next target ###############

INCLUDE(${CMAKE_SOURCE_DIR}/lib/common/sources.cmake)

ADD_LIBRARY(common STATIC ${generated_SRCS} ${common_SRCS} ${codegen_SRCS} )

########### install files ###############




#original Makefile.am contents follow:

## $Id$ $Revision$
### Process this file with automake to produce Makefile.in
#
#AM_CPPFLAGS = \
#        -I$(top_srcdir) \
#	-I$(top_srcdir)/lib/gvc \
#	-I$(top_srcdir)/lib/pack \
#	-I$(top_srcdir)/lib/fdpgen \
#	-I$(top_srcdir)/lib/pathplan \
#	-I$(top_srcdir)/lib/graph \
#	-I$(top_srcdir)/lib/cdt @GD_INCLUDES@ @EXPAT_INCLUDES@ @Z_INCLUDES@
#
#pkginclude_HEADERS = logic.h arith.h geom.h color.h types.h globals.h textpara.h usershape.h
#noinst_HEADERS = render.h utils.h memory.h \
#	geomprocs.h colorprocs.h colortbl.h entities.h \
#	const.h macros.h htmllex.h htmltable.h pointset.h
#noinst_LTLIBRARIES = libcommon_C.la
#
#if WITH_LIBGD
#GD_CODEGENS = gdgen.c vrmlgen.c
#endif
#
#if WITH_CODEGENS
#CODEGENS = $(GD_CODEGENS) diagen.c figgen.c hpglgen.c mapgen.c mifgen.c mpgen.c \
#	picgen.c psgen.c svggen.c vtxgen.c ps.txt xdgen.c color_names
#endif
#
#
#libcommon_C_la_SOURCES = arrows.c colxlate.c fontmetrics.c \
#	args.c memory.c globals.c htmllex.c htmlparse.y htmltable.c input.c \
#	pointset.c postproc.c routespl.c splines.c psusershape.c \
#	timing.c labels.c ns.c shapes.c utils.c geom.c \
#	output.c emit.c $(CODEGENS)
#
#psgen.o psgen.lo : ps.h
#
#ps.h : ps.txt
#	$(AWK) -f $(top_srcdir)/awk/stringize.awk ps.txt > ps.h
#
#colxlate.o colxlate.lo : colortbl.h
#
#colortbl.h : color_lib
#	$(AWK) -f $(top_srcdir)/awk/colortbl.awk color_lib > colortbl.h
#
#color_lib : brewer_lib $(top_srcdir)/lib/common/color_names
#	cat brewer_lib $(top_srcdir)/lib/common/color_names | LC_COLLATE=C $(SORT) > color_lib
#
#brewer_lib : $(top_srcdir)/lib/common/brewer_colors  $(top_srcdir)/awk/brewer.awk
#	$(AWK) -f $(top_srcdir)/awk/brewer.awk $(top_srcdir)/lib/common/brewer_colors > brewer_lib
#
#htmllex.o htmllex.lo: htmllex.c htmllex.h htmlparse.h
#htmlparse.o htmlparse.lo: htmlparse.c htmlparse.h
#                                                                                
#y.output: $(top_srcdir)/lib/common/htmlparse.y
#	@YACC@ -dv $(top_srcdir)/lib/common/htmlparse.y
#
#htmlparse.c: y.output
#	@SED@ "s/yy/html/g" < y.tab.c > htmlparse.c
#
#htmlparse.h: y.output
#	@SED@ "s/yy/html/g" < y.tab.h > htmlparse.h
#
#DISTCLEANFILES = brewer_lib color_lib colortbl.h y.output y.tab.[ch] htmlparse.[ch]
#
#EXTRA_DIST = Makefile.old README.imap chars.tcl ps.h \
#	strcasecmp.c strncasecmp.c htmlparse.c htmlparse.h \
#	y.tab.c y.tab.h y.output entities.html entities.tcl \
#	brewer_colors brewer_lib color_names color_lib colortbl.h
