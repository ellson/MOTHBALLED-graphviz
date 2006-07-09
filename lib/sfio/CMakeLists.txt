# $Id$ $Revision$

INCLUDE(SRCDIRs)
INCLUDE(${sfio_SRCDIR}/sources.cmake)

ADD_LIBRARY(sfio STATIC ${sfio_SRCS})
