# $Id$ $Revision$

INCLUDE(SRCDIRs)
INCLUDE(${agutil_SRCDIR}/sources.cmake)

ADD_LIBRARY(agutil STATIC ${agutil_SRCS})
