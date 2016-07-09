# $Id$ $Revision$
## Process this file with automake to produce Makefile.in

html = gd.html

AM_CPPFLAGS = @FREETYPE2_CFLAGS@ @FONTCONFIG_CFLAGS@ @PNG_INCLUDES@ @JPEG_INCLUDES@ @Z_INCLUDES@

htmldir = $(pkgdatadir)/doc/html

if WITH_MYLIBGD
noinst_HEADERS = gd.h gd_io.h gdcache.h gdfontg.h gdfontl.h gdfontmb.h \
	gdfonts.h gdfontt.h gdhelpers.h jisx0208.h wbmp.h gdfx.h entities.h
noinst_LTLIBRARIES = libgvgd_C.la
html_DATA = $(html)
AM_CPPFLAGS += -DNONDLL
endif

libgvgd_C_la_SOURCES = \
	gdtables.c gd.c gdfx.c gd_security.c gd_gd.c gd_gd2.c gd_io.c \
	gd_io_dp.c gd_gif_in.c gd_gif_out.c gd_io_file.c gd_io_ss.c \
	gd_jpeg.c gd_png.c gd_ss.c gd_topal.c gd_wbmp.c gdcache.c \
	gdfontg.c gdfontl.c gdfontmb.c gdfonts.c gdfontt.c gdft.c \
	gdhelpers.c gdkanji.c gdxpm.c wbmp.c

gd.html: index.html
	cp $(top_srcdir)/lib/gd/index.html gd.html

# keep the contents of the original gd-2.0.31 distribution
EXTRA_DIST = README-JPEG.TXT README.TXT configure.pl bdftogd demoin.png \
	index.html install-item makefile.sample readme.jpn \
	entities.html entities.tcl \
	annotate.c gdparttopng.c gdtopng.c gd2copypal.c gd2topng.c pngtogd.c \
	pngtogd2.c webpng.c gd2togif.c gdcmpgif.c giftogd2.c \
	fontsizetest.c fontwheeltest.c gdtest.c gddemo.c gd2time.c gdtestft.c \
	testac.c circletexttest.c testtr.c fontconfigtest.c gifanimtest.c \
	configure.ac gd.vcxproj*\
	config.hin config test $(html)

DISTCLEANFILES = $(html_DATA)
