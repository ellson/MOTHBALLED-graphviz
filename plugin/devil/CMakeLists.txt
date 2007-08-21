# $Id$ $Revision$

INCLUDE(SRCDIRs)
INCLUDE(${gvplugin_devil_SRCDIR}/sources.cmake)

ADD_LIBRARY(gvplugin_devil STATIC ${gvplugin_devil_SRCS})
ADD_LIBRARY(gvplugin_devil MODULE ${gvplugin_devil_SRCS})
