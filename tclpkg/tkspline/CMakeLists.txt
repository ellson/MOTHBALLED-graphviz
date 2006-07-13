# $Id$ $Revision$

SUBDIRS(demo) 

INCLUDE(SRCDIRs)
INCLUDE(${tkspline_SRCDIR}/sources.cmake)

ADD_LIBRARY(tkspline MODULE ${tkspline_SRCS})

INSTALL_FILES(FILES
	tkspline.pdf
)
INSTALL_FILES(FILES
	tkspline.n
)
