gladedir = $(datadir)/gtktopview/glade
fontsdir = $(datadir)/gtktopview/fonts
icondir = $(datadir)/gtktopview/icons

AM_CPPFLAGS = \
	-DGTKTOPVIEW_GLADE=\""$(gladedir)/gtktopview.glade"\" \
	-DGTKTOPVIEW_FONT=\""$(fontsdir)/arial.tga"\" \
	-DGTKTOPVIEW_ICONSDIR=\""$(iconsdir)"\" \
        -I$(top_srcdir) \
        -I$(top_srcdir)/lib/cgraph \
        -I$(top_srcdir)/lib/cdt \
        -I$(top_srcdir)/lib/filter \
        -I$(top_srcdir)/lib/utilities \
        -I$(top_srcdir)/lib/xdot \
        -I$(top_srcdir)/lib/GUI \
        -I$(top_srcdir)/lib/common \
	$(GTK_CFLAGS) $(GTKGLEXT_CFLAGS) $(GLADE_CFLAGS) $(FREETYPE2_CFLAGS) $(FONTCONFIG_CFLAGS)

noinst_HEADERS = draw.h glTemplate.h materials.h support.h topview.h trackball.h tvnodes.h viewport.h
bin_PROGRAMS = gtktopview

gtktopview_SOURCES = topview.c viewport.c draw.c glTemplate.c main.c support.c template.c trackball.c tvnodes.c

gtktopview_LDADD = $(top_builddir)/lib/cgraph/libcgraph_C.la \
		$(top_builddir)/lib/cdt/libcdt_C.la \
		$(top_builddir)/lib/utilities/libutilities_C.la \
		$(top_builddir)/lib/xdot/libxdot_C.la \
		$(top_builddir)/lib/filter/libfilter_C.la \
		$(top_builddir)/lib/GUI/libGUI_C.la \
		$(top_builddir)/lib/common/libcommon_C.la \
		$(GTK_LIBS) $(GTKGLEXT_LIBS) $(GLADE_LIBS) $(EXPAT_LIBS)
