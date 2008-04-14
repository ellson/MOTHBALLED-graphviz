guidir = $(pkgdatadir)/gui
icondir = $(pkgdatadir)/icons

AM_CPPFLAGS = \
	-DSMYRNA_PATH=\""$(pkgdatadir)"\" \
	-DSMYRNA_GLADE=\""$(guidir)/smyrna.glade"\" \
	-DSMYRNA_ATTRS=\""$(guidir)/attrs.txt"\" \
	-DSMYRNA_FONT=\""$(guidir)/arial.tga"\" \
	-DSMYRNA_ICON_PAN=\""$(icondir)/pan.raw"\" \
	-DSMYRNA_ICON_ZOOM=\""$(icondir)/zoom.raw"\" \
	-DSMYRNA_ICON_ZOOMPLUS=\""$(icondir)/zoomplus.raw"\" \
	-DSMYRNA_ICON_ZOOMMINUS=\""$(icondir)/zoomminus.raw"\" \
	-DSMYRNA_ICON_FISHEYE=\""$(icondir)/fisheye.raw"\" \
        -I$(top_srcdir) \
        -I$(top_srcdir)/lib/cgraph \
        -I$(top_srcdir)/lib/cdt \
        -I$(top_srcdir)/lib/glcomp \
        -I$(top_srcdir)/lib/xdot \
        -I$(top_srcdir)/lib/glcomp \
        -I$(top_srcdir)/lib/topfish \
        -I$(top_srcdir)/lib/common \
        -I$(top_srcdir)/cmd/smyrna/gui \
	$(GTK_CFLAGS) $(GTKGLEXT_CFLAGS) $(GLADE_CFLAGS) $(FREETYPE2_CFLAGS) $(FONTCONFIG_CFLAGS)

if WITH_SMYRNA
noinst_HEADERS = btree.h draw.h filter.h glexpose.h glmotion.h gltemplate.h \
	hier.h materials.h selection.h smyrnadefs.h support.h topview.h \
	topviewdata.h topviewdefs.h trackball.h tvnodes.h viewport.h \
	viewportcamera.h \
	gui/callbacks.h gui/datalistcallbacks.h gui/filterscallbacks.h \
	gui/glmenu.h gui/gui.h gui/menucallbacks.h gui/toolboxcallbacks.h \
	gui/topviewsettings.h
bin_PROGRAMS = smyrna
endif

smyrna_SOURCES = btree.c draw.c filter.c glexpose.c glmotion.c gltemplate.c \
	hier.c main.c selection.c support.c template.c topview.c topviewdata.c \
	trackball.c tvnodes.c viewport.c viewportcamera.c \
	gui/callbacks.c gui/datalistcallbacks.c gui/filterscallbacks.c \
	gui/glmenu.c gui/gui.c gui/menucallbacks.c gui/toolboxcallbacks.c \
	gui/topviewsettings.c

smyrna_LDADD = $(top_builddir)/lib/cgraph/libcgraph_C.la \
		$(top_builddir)/lib/cdt/libcdt_C.la \
		$(top_builddir)/lib/xdot/libxdot_C.la \
		$(top_builddir)/lib/glcomp/libglcomp_C.la \
		$(top_builddir)/lib/topfish/libtopfish_C.la \
		$(top_builddir)/lib/common/libcommon_C.la \
		$(GTK_LIBS) $(GTKGLEXT_LIBS) $(GLADE_LIBS) $(EXPAT_LIBS)
