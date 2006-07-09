# $Id$ $Revision$

ADD_CUSTOM_COMMAND(
	OUTPUT agraph.pdf
	DEPENDS agraph.3
	COMMAND groff -Tps -man agraph.3 | ps2pdf - agraph.pdf
)

INCLUDE(SRCDIRs)
INCLUDE(${agraph_SRCDIR}/sources.cmake)

ADD_LIBRARY(agraph STATIC ${agraph_SRCS})

INSTALL_FILES(FILES
	agraph.pdf
)
INSTALL_FILES(FILES
	agraph.1
)
INSTALL_FILES(FILES
	libagraph.pc
)
