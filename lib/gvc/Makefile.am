## Process this file with automake to produce Makefile.in

pkgconfigdir = $(libdir)/pkgconfig

AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/lib/common \
	-I$(top_srcdir)/lib/pathplan \
	-I$(top_srcdir)/lib/graph \
	-I$(top_srcdir)/lib/cdt $(INCLTDL) \
	-DGVLIBDIR=\"$(pkglibdir)\"

LIBS = $(LIBLTDL) $(SOCKET_LIBS)

pkginclude_HEADERS = gvc.h gvcint.h gvplugin.h gvcproc.h gvplugin_render.h \
	gvplugin_layout.h gvplugin_textlayout.h gvplugin_device.h \
	gvplugin_usershape.h
pkglib_LTLIBRARIES = libgvc.la
pkgconfig_DATA = libgvc.pc

libgvc_la_SOURCES = gvrender.c gvlayout.c gvtextlayout.c gvdevice.c \
	gvcontext.c gvjobs.c gvevent.c gvplugin.c gvconfig.c gvusershape.c \
	gvc.c

libgvc_la_LIBADD = \
	$(top_builddir)/lib/common/libcommon.la \
	$(top_builddir)/lib/graph/libgraph.la \
	$(top_builddir)/lib/pathplan/libpathplan.la \
	$(top_builddir)/lib/plugin/libgvplugin_dot_layout.la \
	$(top_builddir)/lib/plugin/libgvplugin_neato_layout.la \
	$(top_builddir)/lib/plugin/libgvplugin_usershape_gd.la \
	@GD_LIBS@ @EXPAT_LIBS@ @Z_LIBS@ @LIBGEN_LIBS@

EXTRA_DIST = Makefile.old builtins.c
