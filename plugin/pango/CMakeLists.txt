# $Id$ $Revision$

INCLUDE(SRCDIRs)
INCLUDE(${gvplugin_pango_SRCDIR}/sources.cmake)

ADD_LIBRARY(gvplugin_pango MODULE ${gvplugin_pango_SRCS})

ADD_LIBRARY(gvplugin_pango_static STATIC ${gvplugin_pango_SRCS})
