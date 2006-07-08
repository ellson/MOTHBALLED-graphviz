# $Id$ $Revision$

SET(agutil_SRCDIR ${CMAKE_SOURCE_DIR}/lib/agutil)

INCLUDE_DIRECTORIES(${agutil_SRCDIR})

SET(agutil_SRCS
	${agutil_SRCDIR}/dynattr.c
	${agutil_SRCDIR}/nodeq.c
)
