# $Id$ $Revision$
## Process this file with automake to produce Makefile.in

pkgconfigdir = $(libdir)/pkgconfig

AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/lib/common \
	-I$(top_srcdir)/lib/pathplan \
	-I$(top_srcdir)/lib/graph \
	-I$(top_srcdir)/lib/cdt \
	$(INCLTDL) -I$(top_srcdir)/libltdl \
	-DGVLIBDIR=\"$(pkglibdir)\"

LIBS = $(LIBLTDL) $(SOCKET_LIBS)

pkginclude_HEADERS = gvc.h gvcext.h gvplugin.h gvcjob.h gvcint.h \
	gvplugin_render.h \
	gvplugin_layout.h \
	gvplugin_textlayout.h \
	gvplugin_device.h \
	gvplugin_usershape.h
noinst_HEADERS = gvcproc.h
pkglib_LTLIBRARIES = libgvc.la libgvc_builtins.la
pkgconfig_DATA = libgvc.pc

libgvc_builtins_la_LDFLAGS = -version-info @VERSION_INFO@
libgvc_la_LDFLAGS = ${libgvc_builtins_la_LDFLAGS} -no-undefined

libgvc_builtins_la_SOURCES = gvrender.c gvlayout.c gvtextlayout.c gvdevice.c \
	gvcontext.c gvjobs.c gvevent.c gvplugin.c gvconfig.c gvusershape.c \
	gvc.c
libgvc_la_SOURCES = ${libgvc_builtins_la_SOURCES} no_builtins.c

libgvc_builtins_la_LIBADD = \
	$(top_builddir)/lib/cdt/libcdt.la \
	$(top_builddir)/lib/common/libcommon.la \
	$(top_builddir)/lib/graph/libgraph.la \
	$(top_builddir)/lib/pathplan/libpathplan.la \
	@GD_LIBS@ @EXPAT_LIBS@ @Z_LIBS@ @LIBGEN_LIBS@
libgvc_la_LIBADD = ${libgvc_builtins_la_LIBADD}

EXTRA_DIST = Makefile.old dot_builtins.c
