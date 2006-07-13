# $Id$ $Revision$

ADD_SUBDIRECTORY(demo) 

INCLUDE(SRCDIRs)
INCLUDE(${tcldot_SRCDIR}/sources.cmake)
INCLUDE(${gdtclft_SRCDIR}/sources.cmake)
INCLUDE(${gd_SRCDIR}/sources.cmake)
INCLUDE(${tclhandle_SRCDIR}/sources.cmake)
INCLUDE(${tclstubs_SRCDIR}/sources.cmake)

ADD_LIBRARY(tcldot MODULE
	${tcldot_SRCS}
	${gdtclft_SRCS}
	${tclhandle_SRCS}
	${tclstubs_SRCS}
) 

INSTALL_FILES(FILES
	tcldot.pdf
)
INSTALL_FILES(FILES
	tcldot.n
)
INSTALL_FILES(FILES
	tcldot.html
)
