## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/lib/common \
	-I$(top_srcdir)/lib/pathplan \
	-I$(top_srcdir)/lib/graph \
	-I$(top_srcdir)/lib/cdt $(INCLTDL) \
	-DGVLIBDIR=\"$(pkglibdir)\"

LIBS = $(LIBLTDL) $(SOCKET_LIBS)

pkginclude_HEADERS = gvc.h gvcint.h gvplugin.h \
	gvplugin_render.h gvplugin_layout.h gvplugin_textlayout.h \
	gvplugin_device.h gvplugin_usershape.h
pkglib_LTLIBRARIES = libgvc.la

libgvc_la_SOURCES = gvrender.c gvlayout.c gvtextlayout.c gvdevice.c \
	gvcontext.c gvjobs.c gvevent.c gvplugin.c gvconfig.c gvusershape.c

EXTRA_DIST = Makefile.old
