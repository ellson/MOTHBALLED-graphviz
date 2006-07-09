# $Id$ $Revision$

INCLUDE(SRCDIRs)
INCLUDE(${gvplugin_neato_layout_SRCDIR}/sources.cmake)

ADD_LIBRARY(gvplugin_neato_layout MODULE ${gvplugin_neato_layout_SRCS})

ADD_LIBRARY(gvplugin_neato_layout_static STATIC ${gvplugin_neato_layout_SRCS})
