# $Id$ $Revision$
## Process this file with cmake to produce Makefile

INCLUDE( UsePkgConfig )

INCLUDE( FindJPEG )
INCLUDE( FindPNG )
INCLUDE( FindZLIB )

PKGCONFIG( freetype2 FREETYPE_INCLUDE_DIR FREETYPE_LIB_DIR FREETYPE_LINK_FLAGS FREETYPE_CFLAGS )
PKGCONFIG( fontconfig FONTCONFIG_INCLUDE_DIR FONTCONFIG_LIB_DIR FONTCONFIG_LINK_FLAGS FONTCONFIG_CFLAGS )

INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}
	${FREETYPE_INCLUDE_DIR}
	${FONTCONFIG_INCLUDE_DIR}
)



########### next target ###############

ADD_DEFINITIONS(${FREETYPE_CFLAGS} ${FONTCONFIG_CFLAGS})
SET(gvgd_SRCS
	gdtables.c
	gd.c
	gdfx.c
	gd_security.c
	gd_gd.c
	gd_gd2.c
	gd_io.c
	gd_io_dp.c
	gd_gif_in.c
	gd_gif_out.c
	gd_io_file.c
	gd_io_ss.c
	gd_jpeg.c
	gd_png.c
	gd_ss.c
	gd_topal.c
	gd_wbmp.c
	gdcache.c
	gdfontg.c
	gdfontl.c
	gdfontmb.c
	gdfonts.c
	gdfontt.c
	gdft.c
	gdhelpers.c
	gdkanji.c
	gdxpm.c
	wbmp.c
)

ADD_LIBRARY(gvgd STATIC ${gvgd_SRCS})


########### install files ###############

INSTALL_FILES(FILES
	gd.html
)



#original Makefile.am contents follow:

## $Id$ $Revision$
### Process this file with automake to produce Makefile.in
#
#html = gd.html
#
#AM_CPPFLAGS = @FT_INCLUDES@ @FC_INCLUDES@ @PNG_INCLUDES@ @JPEG_INCLUDES@ @Z_INCLUDES@
#
#htmldir = $(pkgdatadir)/doc/html
#
#if WITH_MYLIBGD
#noinst_HEADERS = gd.h gd_io.h gdcache.h gdfontg.h gdfontl.h gdfontmb.h \
#	gdfonts.h gdfontt.h gdhelpers.h jisx0208.h wbmp.h gdfx.h entities.h
#noinst_LTLIBRARIES = libgvgd_C.la
#pkglib_LTLIBRARIES = libgvgd.la
#html_DATA = $(html)
#endif
#
#libgvgd_C_la_SOURCES = \
#	gdtables.c gd.c gdfx.c gd_security.c gd_gd.c gd_gd2.c gd_io.c gd_io_dp.c \
#	gd_gif_in.c gd_gif_out.c gd_io_file.c gd_io_ss.c gd_jpeg.c gd_png.c \
#	gd_ss.c gd_topal.c gd_wbmp.c gdcache.c gdfontg.c gdfontl.c gdfontmb.c \
#	gdfonts.c gdfontt.c gdft.c gdhelpers.c gdkanji.c gdxpm.c wbmp.c
#
#libgvgd_la_SOURCES = $(libgvgd_C_la_SOURCES)
#libgvgd_la_LIBADD = @ICONV_LIBS@ @FC_LIBS@ @FT_LIBS@ @JPEG_LIBS@ @PNG_LIBS@ @Z_LIBS@ @MATH_LIBS@
#
#
#gd.html: index.html
#	cp $(top_srcdir)/lib/gd/index.html gd.html
#
## keep the contents of the original gd-2.0.31 distribution
#EXTRA_DIST = README-JPEG.TXT README.TXT configure.pl bdftogd demoin.png \
#	index.html install-item makefile.sample readme.jpn \
#	entities.html entities.tcl \
#	annotate.c gdparttopng.c gdtopng.c gd2copypal.c gd2topng.c pngtogd.c \
#	pngtogd2.c webpng.c gd2togif.c gdcmpgif.c giftogd2.c \
#	fontsizetest.c fontwheeltest.c gdtest.c gddemo.c gd2time.c gdtestft.c \
#	testac.c circletexttest.c testtr.c fontconfigtest.c gifanimtest.c \
#	configure.ac Makefile.old Makefile.am.orig Makefile.in.orig \
#	config.hin config test $(html)
#
#DISTCLEANFILES = $(html_DATA)
