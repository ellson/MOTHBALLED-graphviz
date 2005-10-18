## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = \
        -I$(top_srcdir) \
	-I$(top_srcdir)/lib/gvc \
	-I$(top_srcdir)/lib/pack \
	-I$(top_srcdir)/lib/fdpgen \
	-I$(top_srcdir)/lib/pathplan \
	-I$(top_srcdir)/lib/graph \
	-I$(top_srcdir)/lib/cdt  @GD_INCLUDES@ @EXPAT_INCLUDES@ @Z_INCLUDES@

pkginclude_HEADERS = logic.h geom.h const.h macros.h types.h globals.h \
	htmllex.h htmltable.h pointset.h render.h renderprocs.h \
	utils.h colortbl.h
noinst_LTLIBRARIES = libcommon.la

if !DISABLE_CODEGENS
CODEGENS = diagen.c figgen.c hpglgen.c gdgen.c mapgen.c mifgen.c mpgen.c \
	picgen.c psgen.c svggen.c vrmlgen.c vtxgen.c ps.txt xdgen.c color_names
endif


libcommon_la_SOURCES = arrows.c colxlate.c fontmetrics.c \
	args.c gdtextsize.c gdusershape.c \
	globals.c htmllex.c htmlparse.y htmltable.c input.c \
	pointset.c postproc.c psusershape.c routespl.c splines.c \
	svgusershape.c timing.c labels.c ns.c shapes.c utils.c geom.c \
	output.c emit.c $(CODEGENS)

psgen.o psgen.lo : ps.h

ps.h : ps.txt
	$(AWK) -f $(top_srcdir)/awk/stringize.awk ps.txt > ps.h

colxlate.o colxlate.lo : colortbl.h

colortbl.h : color_lib
	$(AWK) -f $(top_srcdir)/awk/colortbl.awk color_lib > colortbl.h

color_lib : $(top_srcdir)/lib/common/color_names
	$(SED) s/_//g $(top_srcdir)/lib/common/color_names | LC_COLLATE=C $(SORT) > color_lib

htmllex.o htmllex.lo: htmllex.c htmllex.h htmlparse.h
htmlparse.o htmlparse.lo: htmlparse.c htmlparse.h
                                                                                
y.output: $(top_srcdir)/lib/common/htmlparse.y
	@YACC@ -dv $(top_srcdir)/lib/common/htmlparse.y

htmlparse.c: y.output
	@SED@ "s/yy/html/g" < y.tab.c > htmlparse.c

htmlparse.h: y.output
	@SED@ "s/yy/html/g" < y.tab.h > htmlparse.h

DISTCLEANFILES = color_lib colortbl.h y.output y.tab.[ch] htmlparse.[ch]

EXTRA_DIST = Makefile.old README.imap chars.tcl ps.h \
	strcasecmp.c strncasecmp.c htmlparse.c htmlparse.h y.output
