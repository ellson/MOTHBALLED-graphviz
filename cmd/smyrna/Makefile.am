guidir = $(pkgdatadir)/gui
icondir = $(pkgdatadir)/icons

AM_CPPFLAGS = \
	-DSMYRNA_GLADE=\""$(guidir)/smyrna.glade"\" \
	-DSMYRNA_FONT=\""$(guidir)/arial.tga"\" \
	-DSMYRNA_ICONSDIR=\""$(iconsdir)"\" \
        -I$(top_srcdir) \
        -I$(top_srcdir)/lib/cgraph \
        -I$(top_srcdir)/lib/cdt \
        -I$(top_srcdir)/lib/filter \
        -I$(top_srcdir)/lib/utilities \
        -I$(top_srcdir)/lib/xdot \
        -I$(top_srcdir)/lib/gui \
        -I$(top_srcdir)/lib/common \
	$(GTK_CFLAGS) $(GTKGLEXT_CFLAGS) $(GLADE_CFLAGS) $(FREETYPE2_CFLAGS) $(FONTCONFIG_CFLAGS)

if WITH_SMYRNA
noinst_HEADERS = draw.h glTemplate.h materials.h support.h topview.h trackball.h tvnodes.h viewport.h
bin_PROGRAMS = smyrna
endif

smyrna_SOURCES = topview.c viewport.c draw.c glTemplate.c main.c support.c template.c trackball.c tvnodes.c

smyrna_LDADD = $(top_builddir)/lib/cgraph/libcgraph_C.la \
		$(top_builddir)/lib/cdt/libcdt_C.la \
		$(top_builddir)/lib/utilities/libutilities_C.la \
		$(top_builddir)/lib/xdot/libxdot_C.la \
		$(top_builddir)/lib/filter/libfilter_C.la \
		$(top_builddir)/lib/gui/libgui_C.la \
		$(top_builddir)/lib/common/libcommon_C.la \
		$(GTK_LIBS) $(GTKGLEXT_LIBS) $(GLADE_LIBS) $(EXPAT_LIBS)
