# $Id$ $Revision$

ADD_CUSTOM_COMMAND(
	OUTPUT cdt.pdf
	DEPENDS cdt.3
	COMMAND groff -Tps -man cdt.3 | ps2pdf - cdt.pdf
)

INCLUDE(${CMAKE_SOURCE_DIR}/cmake/directories.cmake)
INCLUDE(${cdt_SRCDIR}/sources.cmake)

ADD_LIBRARY(cdt STATIC ${cdt_SRCS})

INSTALL_FILES(FILES
	libcdt.pc
)
INSTALL_FILES(FILES
	cdt.pdf
)
INSTALL_FILES(FILES
	cdt.1
)
