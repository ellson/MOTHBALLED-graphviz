## Process this file with automake to produce Makefile.in -*-Makefile-*-
AUTOMAKE_OPTIONS = foreign 1.7

ACLOCAL_AMFLAGS = -I config

SUBDIRS = config test

bin_PROGRAMS = annotate gdparttopng gdtopng gd2copypal gd2topng pngtogd pngtogd2 webpng gd2togif gdcmpgif giftogd2

bin_SCRIPTS = bdftogd config/gdlib-config

noinst_PROGRAMS = fontsizetest fontwheeltest gdtest gddemo gd2time gdtestft testac circletexttest testtr fontconfigtest gifanimtest

EXTRA_DIST = README-JPEG.TXT README.TXT configure.pl bdftogd demoin.png err.out index.html install-item makefile.sample readme.jpn entities.html entities.tcl

include_HEADERS = gd.h gdfx.h gd_io.h gdcache.h gdfontg.h gdfontl.h gdfontmb.h gdfonts.h gdfontt.h entities.h

lib_LTLIBRARIES = libgd.la

libgd_la_SOURCES = gd.c gdfx.c gd_security.c gd_gd.c gd_gd2.c gd_io.c gd_io_dp.c gd_gif_in.c gd_gif_out.c gd_io_file.c gd_io_ss.c gd_jpeg.c gd_png.c gd_ss.c gd_topal.c gd_wbmp.c gdcache.c gdfontg.c gdfontl.c gdfontmb.c gdfonts.c gdfontt.c gdft.c gdhelpers.c gdhelpers.h gdkanji.c gdtables.c gdxpm.c jisx0208.h wbmp.c wbmp.h

libgd_la_LDFLAGS = -version-info 2:0:0

LDADD = ./libgd.la

dist-zip: distdir
	zip -qr $(distdir).zip $(distdir)
	$(am__remove_distdir)
