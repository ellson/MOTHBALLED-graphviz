# $Id$ $Revision$

INCLUDE(FindTCL)

INCLUDE_DIRECTORIES(
	${gdtclft_SRCDIR}
	${top_SRCDIR}
	${tclhandle_SRCDIR}
	${TCL_INCLUDE_PATH}
)

SET(gdtclft_SRCS
	${gdtclft_SRCDIR}/gdtclft.c
)
