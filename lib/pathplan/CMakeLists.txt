# $Id$ $Revision$

ADD_CUSTOM_COMMAND(
	OUTPUT pathplan.pdf
	DEPENDS pathplan.3
	COMMAND groff -Tps -man pathplan.3 | ps2pdf - pathplan.pdf
)

INCLUDE(SRCDIRs)
INCLUDE(${pathplan_SRCDIR}/sources.cmake)

ADD_LIBRARY(pathplan STATIC ${pathplan_SRCS})
ADD_LIBRARY(pathplan SHARED ${pathplan_SRCS})

INSTALL_FILES(FILES
	pathplan.pdf
)
INSTALL_FILES(FILES
	pathplan.3
)
INSTALL_FILES(FILES
	libpathplan.pc
)
