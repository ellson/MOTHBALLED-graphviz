# $Id$ $Revision$

INCLUDE(SRCDIRs)
INCLUDE(${gvplugin_gdk_pixbuf_SRCDIR}/sources.cmake)

ADD_LIBRARY(gvplugin_gdk_pixbuf STATIC ${gvplugin_gdk_pixbuf_SRCS})
ADD_LIBRARY(gvplugin_gdk_pixbuf MODULE ${gvplugin_gdk_pixbuf_SRCS})
