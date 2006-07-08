# $Id$ $Revision$

ADD_CUSTOM_COMMAND(
	OUTPUT graph.pdf
	DEPENDS graph.3
	COMMAND groff -Tps -man graph.3 | ps2pdf - graph.pdf
)

INCLUDE(${CMAKE_SOURCE_DIR}/cmake/directories.cmake)
INCLUDE(${graph_SRCDIR}/sources.cmake)

ADD_LIBRARY(graph STATIC ${graph_SRCS})

INSTALL_FILES(FILES
	graph.pdf
)
INSTALL_FILES(FILES
	graph.3
)
INSTALL_FILES(FILES
	libgraph.pc
)
