# $Id$ $Revision$

INCLUDE(FindTCL)
INCLUDE(SRCDIRs)
INCLUDE(${tclstubs_SRCDIR}/sources.cmake)

INCLUDE_DIRECTORIES(
	${tclstubs_SRCDIR}
	${TCL_INCLUDE_PATH}
)

ADD_LIBRARY(tclstubs STATIC ${tclstubs_SRCS})
