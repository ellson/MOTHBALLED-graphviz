# $Id$ $Revision$
## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = \
        -I$(top_srcdir) \
	-I$(top_srcdir)/lib/gvc \
	-I$(top_srcdir)/lib/pack \
	-I$(top_srcdir)/lib/fdpgen \
	-I$(top_srcdir)/lib/pathplan \
	-I$(top_srcdir)/lib/graph \
	-I$(top_srcdir)/lib/cdt @GD_INCLUDES@ @EXPAT_INCLUDES@ @Z_INCLUDES@

pkginclude_HEADERS = logic.h arith.h geom.h color.h types.h globals.h textpara.h usershape.h
noinst_HEADERS = render.h utils.h memory.h \
	geomprocs.h colorprocs.h colortbl.h entities.h \
	const.h macros.h htmllex.h htmltable.h pointset.h
noinst_LTLIBRARIES = libcommon_C.la

if WITH_CODEGENS
CODEGENS = $(GD_CODEGENS) diagen.c hpglgen.c mifgen.c mpgen.c picgen.c vtxgen.c
endif


libcommon_C_la_SOURCES = arrows.c colxlate.c fontmetrics.c \
	args.c memory.c globals.c htmllex.c htmlparse.y htmltable.c input.c \
	pointset.c postproc.c routespl.c splines.c psusershape.c \
	timing.c labels.c ns.c shapes.c utils.c geom.c \
	output.c emit.c ps.txt ps_font_equiv.txt color_names $(CODEGENS)

fontmetrics.o fontmetrics.lo: ps_font_equiv.h

# ensure font names are properly sorted for bsearch operation 
ps_font_equiv.h: $(srcdir)/ps_font_equiv.txt
	LC_ALL=C $(SORT) <$(srcdir)/ps_font_equiv.txt >ps_font_equiv.h

utils.o utils.lo : ps.h

ps.h : $(srcdir)/ps.txt
	$(AWK) -f $(top_srcdir)/awk/stringize.awk $(srcdir)/ps.txt > ps.h

colxlate.o colxlate.lo : colortbl.h

colortbl.h : color_lib
	$(AWK) -f $(top_srcdir)/awk/colortbl.awk color_lib > colortbl.h

# ensure color names are properly sorted for bsearch operation 
color_lib : brewer_lib $(top_srcdir)/lib/common/color_names
	cat brewer_lib $(top_srcdir)/lib/common/color_names | LC_ALL=C $(SORT) > color_lib

brewer_lib : $(top_srcdir)/lib/common/brewer_colors  $(top_srcdir)/awk/brewer.awk
	$(AWK) -f $(top_srcdir)/awk/brewer.awk $(top_srcdir)/lib/common/brewer_colors > brewer_lib

htmllex.o htmllex.lo: htmllex.c htmllex.h htmlparse.h
htmlparse.o htmlparse.lo: htmlparse.c htmlparse.h
                                                                                
y.output: $(top_srcdir)/lib/common/htmlparse.y
	@YACC@ -dv $(top_srcdir)/lib/common/htmlparse.y

htmlparse.c: y.output
	@SED@ "s/yy/html/g" < y.tab.c > htmlparse.c

htmlparse.h: y.output
	@SED@ "s/yy/html/g" < y.tab.h > htmlparse.h

DISTCLEANFILES = brewer_lib color_lib colortbl.h ps_font_equiv.h ps.h \
	y.output y.tab.[ch] htmlparse.[ch]

EXTRA_DIST = Makefile.old README.imap chars.tcl ps.h ps_font_equiv.h \
	strcasecmp.c strncasecmp.c htmlparse.c htmlparse.h \
	y.tab.c y.tab.h y.output entities.html entities.tcl \
	brewer_colors brewer_lib color_names color_lib colortbl.h
