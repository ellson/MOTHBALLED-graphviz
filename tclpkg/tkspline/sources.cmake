# $Id$ $Revision$

INCLUDE(FindTCL)

INCLUDE_DIRECTORIES(
        ${tkspline_SRCDIR}}
        ${top_SRCDIR}
        ${TK_INCLUDE_PATH}
        ${tkstubs_SRCDIR}
)

SET(tkspline_SRCS
	${tkspline_SRCDIR}/tkspline.c
)
