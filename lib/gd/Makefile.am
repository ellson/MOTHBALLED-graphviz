# $Id$ $Revision$
## Process this file with automake to produce Makefile.in

html = gd.html

AM_CPPFLAGS = @FT_INCLUDES@ @FC_INCLUDES@ @PNG_INCLUDES@ @JPEG_INCLUDES@ @Z_INCLUDES@

htmldir = $(pkgdatadir)/doc/html

if WITH_MYLIBGD
noinst_HEADERS = gd.h gd_io.h gdcache.h gdfontg.h gdfontl.h gdfontmb.h \
	gdfonts.h gdfontt.h gdhelpers.h jisx0208.h wbmp.h gdfx.h entities.h
noinst_LTLIBRARIES = libgvgd_C.la
noinst_PROGRAMS = annotate gdparttopng gdtopng gd2copypal gd2topng pngtogd \
	pngtogd2 webpng gd2togif gdcmpgif giftogd2 \
	fontsizetest fontwheeltest gdtest gddemo gd2time gdtestft \
	testac circletexttest testtr fontconfigtest gifanimtest
html_DATA = $(html)
endif

libgvgd_C_la_SOURCES = \
	gdtables.c gd.c gdfx.c gd_security.c gd_gd.c gd_gd2.c gd_io.c gd_io_dp.c \
	gd_gif_in.c gd_gif_out.c gd_io_file.c gd_io_ss.c gd_jpeg.c gd_png.c \
	gd_ss.c gd_topal.c gd_wbmp.c gdcache.c gdfontg.c gdfontl.c gdfontmb.c \
	gdfonts.c gdfontt.c gdft.c gdhelpers.c gdkanji.c gdxpm.c wbmp.c
libgvgd_C_la_LIBADD = @ICONV_LIBS@ @FC_LIBS@ @FT_LIBS@ @JPEG_LIBS@ @PNG_LIBS@ @Z_LIBS@ @MATH_LIBS@

gd.html: index.html
	cp $(top_srcdir)/lib/gd/index.html gd.html

LDADD = ./libgvgd_C.la

dist-zip: distdir
	zip -qr $(distdir).zip $(distdir)
	$(am__remove_distdir)

# keep the contents of the original gd-2.0.31 distribution
EXTRA_DIST = README-JPEG.TXT README.TXT configure.pl bdftogd demoin.png \
	index.html install-item makefile.sample readme.jpn \
	entities.html entities.tcl \
	configure.ac Makefile.old Makefile.am.orig Makefile.in.orig \
	config.hin config test $(html)

DISTCLEANFILES = $(html_DATA)
