# $Id$ $Revision$

INCLUDE(SRCDIRs)
INCLUDE(${gvplugin_gtk_SRCDIR}/sources.cmake)

ADD_LIBRARY(gvplugin_gtk STATIC ${gvplugin_gtk_SRCS})
ADD_LIBRARY(gvplugin_gtk MODULE ${gvplugin_gtk_SRCS})
