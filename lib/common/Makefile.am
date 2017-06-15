# $Id$ $Revision$
## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = \
        -I$(top_srcdir) \
        -I$(top_srcdir)/libltdl \
	-I$(top_srcdir)/lib/gvc \
	-I$(top_srcdir)/lib/pack \
	-I$(top_srcdir)/lib/label \
	-I$(top_srcdir)/lib/xdot \
	-I$(top_srcdir)/lib/fdpgen \
	-I$(top_srcdir)/lib/pathplan \
	-I$(top_srcdir)/lib/cgraph \
	-I$(top_srcdir)/lib/cdt $(GD_INCLUDES) $(EXPAT_INCLUDES) $(Z_INCLUDES)

if WITH_WIN32
AM_CFLAGS = -D_BLD_gvc=1
AM_CPPFLAGS += -O0
endif

BUILT_SOURCES = colortbl.h ps_font_equiv.h htmlparse.h

pkginclude_HEADERS = arith.h geom.h color.h types.h textspan.h usershape.h
noinst_HEADERS = render.h utils.h memory.h \
	geomprocs.h colorprocs.h colortbl.h entities.h globals.h \
	logic.h const.h macros.h htmllex.h htmltable.h pointset.h intset.h \
	timing.h
noinst_LTLIBRARIES = libcommon_C.la

libcommon_C_la_SOURCES = arrows.c colxlate.c ellipse.c textspan.c \
	args.c memory.c globals.c htmllex.c htmlparse.y htmltable.c input.c \
	pointset.c intset.c postproc.c routespl.c splines.c psusershape.c \
	timing.c labels.c ns.c shapes.c utils.c geom.c taper.c \
	output.c emit.c ps_font_equiv.txt ps_fontmap.txt fontmap.cfg \
	color_names

textfont.o textfont.lo: ps_font_equiv.h

# ensure font names are properly sorted for bsearch operation 
ps_font_equiv.h: $(srcdir)/ps_font_equiv.txt $(srcdir)/fontmap.cfg $(srcdir)/ps_fontmap.txt
	cp $(srcdir)/ps_fontmap.txt ps_font_equiv.h
	./mksvgfonts.pl fontmap.cfg ps_font_equiv.txt | LC_ALL=C $(SORT) >>ps_font_equiv.h

colxlate.o colxlate.lo : colortbl.h

colortbl.h : color_lib
	$(AWK) -f $(top_srcdir)/awk/colortbl.awk color_lib > colortbl.h

# ensure color names are properly sorted for bsearch operation 
color_lib : brewer_lib svgcolor_lib $(top_srcdir)/lib/common/color_names
	cat brewer_lib svgcolor_lib $(top_srcdir)/lib/common/color_names | LC_ALL=C $(SORT) > color_lib

brewer_lib : $(top_srcdir)/lib/common/brewer_colors  $(top_srcdir)/awk/brewer.awk
	$(AWK) -f $(top_srcdir)/awk/brewer.awk $(top_srcdir)/lib/common/brewer_colors > brewer_lib

svgcolor_lib : $(top_srcdir)/lib/common/svgcolor_names  $(top_srcdir)/awk/svgcolor.awk
	 $(AWK) -f $(top_srcdir)/awk/svgcolor.awk $(top_srcdir)/lib/common/svgcolor_names > svgcolor_lib

htmllex.o htmllex.lo: htmllex.c htmllex.h htmlparse.h
htmlparse.o htmlparse.lo: htmlparse.c htmlparse.h
                                                                                
htmlparse.c: y.tab.c
	@SED@ "s/yy/html/g" < y.tab.c | \
		@SED@ "s/unsigned long int/uint64_t/g" | \
                @SED@ "s/unsigned long/uint64_t/g" > htmlparse.c

htmlparse.h: y.tab.h
	@SED@ "s/yy/html/g" < y.tab.h | \
		@SED@ "s/unsigned long int/uint64_t/g" | \
                @SED@ "s/unsigned long/uint64_t/g"> htmlparse.h

y.tab.c y.tab.h: y.output

y.output: $(top_srcdir)/lib/common/htmlparse.y
	@YACC@ -dv $(top_srcdir)/lib/common/htmlparse.y

DISTCLEANFILES = brewer_lib color_lib colortbl.h ps_font_equiv.h \
	y.output y.tab.[ch] htmlparse.[ch]

EXTRA_DIST = README.imap chars.tcl ps_font_equiv.h \
	strcasecmp.c strncasecmp.c htmlparse.c htmlparse.h \
	y.tab.c y.tab.h y.output entities.html entities.tcl \
	brewer_colors brewer_lib svgcolor_names svgcolor_lib \
	color_names color_lib colortbl.h mksvgfonts.pl
