# $Id$ $Revision$

ADD_CUSTOM_COMMAND(
	OUTPUT pack.pdf
	DEPENDS pack.3
	COMMAND groff -Tps -man pack.3 | ps2pdf - pack.pdf
)

INCLUDE(SRCDIRs)
INCLUDE(${pack_SRCDIR}/sources.cmake)

ADD_LIBRARY(pack STATIC ${pack_SRCS})

INSTALL_FILES(FILES
	pack.pdf
)
INSTALL_FILES(FILES
	pack.3
)
