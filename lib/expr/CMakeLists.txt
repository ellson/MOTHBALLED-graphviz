# $Id$ $Revision$

INCLUDE(${CMAKE_SOURCE_DIR}/cmake/directories.cmake)
INCLUDE(${expr_SRCDIR}/sources.cmake)

ADD_LIBRARY(expr STATIC ${expr_SRCS})

INSTALL_FILES(FILES
	expr.pdf
)
INSTALL_FILES(FILES
	expr.1
)
