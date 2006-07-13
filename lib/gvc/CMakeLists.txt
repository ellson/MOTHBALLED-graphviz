# $Id$ $Revision$

INCLUDE(SRCDIRs)
INCLUDE(${gvc_SRCDIR}/sources.cmake)
INCLUDE(${common_SRCDIR}/sources.cmake)

ADD_LIBRARY(gvc STATIC ${gvc_SRCS} no_builtins.c demand_loading.c)
#ADD_LIBRARY(gvc SHARED ${gvc_SRCS} ${common_SRCS} no_builtins.c demand_loading.c)
#ADD_LIBRARY(gvc_builtins STATIC ${gvc_SRCS} )

INSTALL_FILES(FILES
	libgvc.pc
	libgvc_builtins.pc
)
