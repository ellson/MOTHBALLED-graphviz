# $Id$ $Revision$

INCLUDE(SRCDIRs)
INCLUDE(${gd_SRCDIR}/sources.cmake)

# rename private copy of libgd to libgvgd
SET(gvgd_SRCS ${gd_SRCS})

ADD_LIBRARY(gvgd STATIC ${gvgd_SRCS})

INSTALL_FILES(FILES
	gd.html
)
