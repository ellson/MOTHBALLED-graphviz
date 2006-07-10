# $Id$ $Revision$

INCLUDE(SRCDIRs)
INCLUDE(${expr_SRCDIR}/sources.cmake)

ADD_LIBRARY(expr STATIC ${expr_SRCS})
ADD_LIBRARY(expr SHARED ${expr_SRCS})

INSTALL_FILES(FILES
	expr.pdf
)
INSTALL_FILES(FILES
	expr.1
)
