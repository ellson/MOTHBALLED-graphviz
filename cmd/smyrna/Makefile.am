AM_CPPFLAGS = \
	-DSMYRNA_PATH=\""$(pkgdatadir)/smyrna"\" \
        -I$(top_srcdir) \
        -I$(top_srcdir)/lib/cgraph \
        -I$(top_srcdir)/lib/cdt \
        -I$(top_srcdir)/lib/glcomp \
        -I$(top_srcdir)/lib/utilities \
        -I$(top_srcdir)/lib/xdot \
        -I$(top_srcdir)/lib/glcomp \
        -I$(top_srcdir)/lib/neatogen \
        -I$(top_srcdir)/lib/topfish \
        -I$(top_srcdir)/lib/gvpr \
        -I$(top_srcdir)/lib/common \
        -I$(top_srcdir)/cmd/smyrna/gui \
	$(GTK_CFLAGS) $(GLUT_CFLAGS) $(GTKGLEXT_CFLAGS) $(GLADE_CFLAGS) $(FREETYPE2_CFLAGS) $(FONTCONFIG_CFLAGS) $(GTS_CFLAGS)

bin_PROGRAMS =
if WITH_SMYRNA
if ENABLE_SHARED
bin_PROGRAMS += smyrna 
endif
if ENABLE_STATIC
bin_PROGRAMS += smyrna_static
endif
endif

noinst_HEADERS = btree.h draw.h filter.h glexpose.h glmotion.h gltemplate.h \
	hier.h materials.h selection.h smyrnadefs.h support.h topview.h \
	topviewdata.h topviewdefs.h trackball.h tvnodes.h viewport.h \
	topfisheyeview.h viewportcamera.h gvprpipe.h md5.h drawxdot.h arcball.h \
	gui/beacon.h gui/callbacks.h gui/datalistcallbacks.h gui/filterscallbacks.h \
	gui/frmobjectui.h gui/glcompui.h gui/gui.h gui/menucallbacks.h \
	gui/toolboxcallbacks.h gui/topviewsettings.h gui/appmouse.h hotkeymap.h

smyrna_SOURCES = btree.c draw.c filter.c glexpose.c glmotion.c gltemplate.c \
	hier.c main.c selection.c support.c template.c topview.c topviewdata.c \
	topfisheyeview.c trackball.c tvnodes.c viewport.c viewportcamera.c \
	gvprpipe.c md5.c drawxdot.c arcball.c \
	gui/beacon.c gui/callbacks.c gui/datalistcallbacks.c gui/filterscallbacks.c \
	gui/frmobjectui.c gui/glcompui.c gui/gui.c gui/menucallbacks.c \
	gui/toolboxcallbacks.c gui/topviewsettings.c gui/appmouse.c hotkeymap.c

smyrna_LDADD = $(top_builddir)/lib/cgraph/libcgraph_C.la \
		$(top_builddir)/lib/cdt/libcdt_C.la \
		$(top_builddir)/lib/xdot/libxdot_C.la \
		$(top_builddir)/lib/glcomp/libglcomp_C.la \
		$(top_builddir)/lib/topfish/libtopfish_C.la \
		$(top_builddir)/lib/common/libcommon_C.la \
		$(top_builddir)/lib/gvpr/libgvpr_C.la \
		$(top_builddir)/lib/expr/libexpr_C.la \
		$(top_builddir)/lib/ingraphs/libingraphs_C.la \
		$(top_builddir)/lib/neatogen/libneatogen_C.la \
		$(GTK_LIBS) $(GLUT_LIBS) $(GTKGLEXT_LIBS) $(GLADE_LIBS) $(EXPAT_LIBS) $(GTS_LIBS) \
		$(EXTRA_SMYRNA_LDFLAGS)

smyrna_static_SOURCES = $(smyrna_SOURCES)
smyrna_static_LDADD = $(top_builddir)/lib/cgraph/libcgraph_C.la \
		$(top_builddir)/lib/cdt/libcdt_C.la \
		$(top_builddir)/lib/xdot/libxdot_C.la \
		$(top_builddir)/lib/glcomp/libglcomp_C.la \
		$(top_builddir)/lib/topfish/libtopfish_C.la \
		$(top_builddir)/lib/common/libcommon_C.la \
		$(top_builddir)/lib/gvpr/libgvpr_C.la \
		$(top_builddir)/lib/expr/libexpr_C.la \
		$(top_builddir)/lib/ingraphs/libingraphs_C.la \
		$(top_builddir)/lib/neatogen/libneatogen_C.la \
		$(GTK_LIBS) $(GLUT_LIBS) $(GTKGLEXT_LIBS) $(GLADE_LIBS) $(EXPAT_LIBS) $(GTS_LIBS)

EXTRA_DIST = smyrna.vcproj
