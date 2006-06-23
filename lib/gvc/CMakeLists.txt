# $Id$ $Revision$
## Process this file with cmake to produce Makefile

INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}/lib/common
	${CMAKE_SOURCE_DIR}/lib/pathplan
	${CMAKE_SOURCE_DIR}/lib/graph
	${CMAKE_SOURCE_DIR}/lib/cdt
)

ADD_DEFINITIONS(-DGVLIBDIR="\\"\\"")


########### next target ###############

SET(gvc_SRCS
	gvrender.c
	gvlayout.c
	gvdevice.c
	gvloadimage.c
	gvcontext.c
	gvjobs.c
	gvevent.c
	gvplugin.c
	gvconfig.c
	gvtextlayout.c
	gvusershape.c
	gvc.c
)

ADD_LIBRARY(gvc STATIC ${gvc_SRCS} no_builtins.c demand_loading.c)

ADD_LIBRARY(gvc_builtins STATIC ${gvc_SRCS} )


########### install files ###############

INSTALL_FILES(FILES
	libgvc.pc
	libgvc_builtins.pc
)



#original Makefile.am contents follow:

## $Id$ $Revision$
### Process this file with automake to produce Makefile.in
#
#pkgconfigdir = $(libdir)/pkgconfig
#
#AM_CPPFLAGS = \
#	-I$(top_srcdir) \
#	-I$(top_srcdir)/lib/common \
#	-I$(top_srcdir)/lib/pathplan \
#	-I$(top_srcdir)/lib/graph \
#	-I$(top_srcdir)/lib/cdt \
#	$(INCLTDL) -I$(top_srcdir)/libltdl \
#	-DGVLIBDIR=\"$(pkglibdir)\"
#
#LIBS = $(LIBLTDL) $(SOCKET_LIBS)
#
#pkginclude_HEADERS = gvc.h gvcext.h gvplugin.h gvcjob.h gvcint.h \
#	gvcommon.h gvplugin_render.h gvplugin_layout.h \
#	gvplugin_textlayout.h gvplugin_device.h gvplugin_loadimage.h \
#	dot_builtins.c demand_loading.c no_demand_loading.c
#noinst_HEADERS = gvcproc.h
#noinst_LTLIBRARIES = libgvc_C.la
#pkglib_LTLIBRARIES = libgvc.la libgvc_builtins.la
#pkgconfig_DATA = libgvc.pc libgvc_builtins.pc
#
#libgvc_C_la_SOURCES = gvrender.c gvlayout.c gvdevice.c gvloadimage.c gvcontext.c gvjobs.c \
#	gvevent.c gvplugin.c gvconfig.c gvtextlayout.c gvusershape.c gvc.c
#libgvc_C_la_LIBADD = $(top_builddir)/lib/common/libcommon_C.la
#
##For use with plugins.
##   so it is linked with an empty table of builtins.
#libgvc_la_LDFLAGS = -version-info @VERSION_INFO@ 
#libgvc_la_SOURCES = $(libgvc_C_la_SOURCES) no_builtins.c demand_loading.c
#if WITH_MYLIBGD
#libgvc_la_LIBADD = $(libgvc_C_la_LIBADD) \
#	$(top_builddir)/lib/gd/libgvgd_C.la \
#	$(top_builddir)/lib/cdt/libcdt.la \
#	$(top_builddir)/lib/graph/libgraph.la \
#	$(top_builddir)/lib/pathplan/libpathplan.la \
#	@ICONV_LIBS@ @FC_LIBS@ @FT_LIBS@ @JPEG_LIBS@ @PNG_LIBS@ @EXPAT_LIBS@ @Z_LIBS@ 
#else
#libgvc_la_LIBADD = $(libgvc_C_la_LIBADD) \
#	$(top_builddir)/lib/cdt/libcdt.la \
#	$(top_builddir)/lib/graph/libgraph.la \
#	$(top_builddir)/lib/pathplan/libpathplan.la \
#	@GD_LIBS@
#endif
#
##For use without plugins.
##  so it needs to be linked with a preset table of builtins, e.g. dot_builtins.c,
##  		with either demand_loading.c or no_demand_loading.c,
##  		and with the plugins themselves, e.g.
##  -lgvplugin_dot_layout -lgvplugin_neato_layout -lgvplugin_gd -lgvplugin_pangocairo
#libgvc_builtins_la_LDFLAGS = -version-info @VERSION_INFO@
#libgvc_builtins_la_SOURCES = $(libgvc_C_la_SOURCES)
#libgvc_builtins_la_LIBADD = $(libgvc_la_LIBADD)
#
#EXTRA_DIST = Makefile.old
