## Process this file with automake to produce Makefile.in

ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS = $(GVC_CFLAGS) $(GTK_CFLAGS)

noinst_HEADERS = support.h interface.h callbacks.h
noinst_LTLIBRARIES = libgvplugin_gtk_C.la
pkglib_LTLIBRARIES = libgvplugin_gtk.la

libgvplugin_gtk_C_la_SOURCES = \
	gvplugin_gtk.c \
	gvdevice_gtk.c \
	support.c \
        interface.c \
        callbacks.c 

libgvplugin_gtk_la_LDFLAGS = -version-info @GVPLUGIN_VERSION_INFO@
libgvplugin_gtk_la_SOURCES = $(libgvplugin_gtk_C_la_SOURCES)
libgvplugin_gtk_la_LIBADD = $(GTK_LIBS)

EXTRA_DIST = gtk.glade gtk.gladep 

# run "dot -c", if possible, to create plugin config
install-exec-hook:
	if test "x$(DESTDIR)" = "x"; then if test -x $(bindir)/dot$(EXEEXT); then if test -x /sbin/ldconfig; then /sbin/ldconfig 2>/dev/null; fi; $(bindir)/dot$(EXEEXT) -c; else $(bindir)/dot_static$(EXEEXT) -c; fi; fi
#
