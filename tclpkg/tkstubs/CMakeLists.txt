# $Id$ $Revision$

INCLUDE(FindTCL)
INCLUDE(SRCDIRs)
INCLUDE(${tkstubs_SRCDIR}/sources.cmake)

INCLUDE_DIRECTORIES(
	${tkstubs_SRCDIR}
	${TCL_INCLUDE_PATH}
	${TK_INCLUDE_PATH}
)

ADD_LIBRARY(tkstubs STATIC ${tkstubs_SRCS})
