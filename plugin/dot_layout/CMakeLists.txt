# $Id$ $Revision$

INCLUDE(SRCDIRs)
INCLUDE(${gvplugin_dot_layout_SRCDIR}/sources.cmake)
INCLUDE(${dotgen_SRCDIR}/sources.cmake)

ADD_LIBRARY(gvplugin_dot_layout STATIC ${gvplugin_dot_layout_SRCS})
ADD_LIBRARY(gvplugin_dot_layout MODULE ${gvplugin_dot_layout_SRCS} ${dotgen_SRCS})
