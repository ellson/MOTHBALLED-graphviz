# $Id$ $Revision$

INCLUDE(SRCDIRs)
INCLUDE(${gvplugin_xlib_SRCDIR}/sources.cmake)

ADD_LIBRARY(gvplugin_xlib MODULE ${gvplugin_xlib_SRCS})

ADD_LIBRARY(gvplugin_xlib_static STATIC ${gvplugin_xlib_SRCS})
