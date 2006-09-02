# $Id$ $Revision$

INCLUDE(SRCDIRs)
INCLUDE(${gvplugin_ming_SRCDIR}/sources.cmake)

ADD_LIBRARY(gvplugin_ming STATIC ${gvplugin_ming_SRCS})
ADD_LIBRARY(gvplugin_ming MODULE ${gvplugin_ming_SRCS})
