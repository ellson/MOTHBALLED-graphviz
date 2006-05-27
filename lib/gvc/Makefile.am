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
	gvplugin_device.h
noinst_HEADERS = gvcproc.h
noinst_LTLIBRARIES = libgvc_C.la
pkglib_LTLIBRARIES = libgvc.la libgvc_builtins.la
pkgconfig_DATA = libgvc.pc

libgvc_C_la_SOURCES = gvrender.c gvlayout.c gvdevice.c \
	gvcontext.c gvjobs.c gvevent.c gvplugin.c gvconfig.c \
	gvtextlayout.c gvusershape.c gvc.c
libgvc_C_la_LIBADD = \
	$(top_builddir)/lib/common/libcommon_C.la

#libgvc is intended for use with plugins
#   so it is linked with an empty table of builtins.
libgvc_la_LDFLAGS = -version-info @VERSION_INFO@ -no-undefined
libgvc_la_SOURCES = $(libgvc_C_la_SOURCES) no_builtins.c
libgvc_la_LIBADD = $(libgvc_C_la_LIBADD) \
	$(top_builddir)/lib/cdt/libcdt.la \
	$(top_builddir)/lib/graph/libgraph.la \
	$(top_builddir)/lib/pathplan/libpathplan.la \
	@GD_LIBS@ @EXPAT_LIBS@ @Z_LIBS@ @LIBGEN_LIBS@

#libgvc_builtins is intended for use with builtins (still allowing plugins).
#   The application needs to provide a table of builtins, dot_builtins.c
#   provides and example.
libgvc_builtins_la_LDFLAGS = -version-info @VERSION_INFO@
libgvc_builtins_la_SOURCES = $(libgvc_C_la_SOURCES)
libgvc_builtins_la_LIBADD = $(libgvc_la_LIBADD)

EXTRA_DIST = Makefile.old dot_builtins.c
