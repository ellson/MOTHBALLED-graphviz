# $Id$ $Revision$
## Process this file with cmake to produce Makefile

INCLUDE( SRCDIRs )

INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_SOURCE_DIR}
	${top_SRCDIR}
	${common_SRCDIR}
	${gvc_SRCDIR}
	${pathplan_SRCDIR}
	${graph_SRCDIR}
	${cdt_SRCDIR}
)

LINK_DIRECTORIES(
	${CMAKE_BINARY_DIR}/lib/common
	${CMAKE_BINARY_DIR}/lib/gvc
	${CMAKE_BINARY_DIR}/lib/pathplan
	${CMAKE_BINARY_DIR}/lib/graph
	${CMAKE_BINARY_DIR}/lib/cdt
	${CMAKE_BINARY_DIR}/lib/gd
	$(PANGOCAIRO_LIB_DIR)
)

########### next target ###############

SET(dot_SRCS
	dot.c
)

ADD_EXECUTABLE(dot ${dot_SRCS})

TARGET_LINK_LIBRARIES(dot
	gvc
	pathplan
	graph
	cdt
	expat
	ltdl
	z
	m
)

INSTALL_TARGETS(/bin dot )


########### next target ###############

# SET(dot_static_SRCS
# 	dot.c
# 	${CMAKE_SOURCE_DIR}/lib/gvc/dot_builtins.c
# 	${CMAKE_SOURCE_DIR}/lib/gvc/no_demand_loading.c
# )
# 
# ADD_EXECUTABLE(dot_static ${dot_static_SRCS})
# 
# TARGET_LINK_LIBRARIES(dot_static
# 	common
# 	gvc_builtins
# 	gvplugin_dot_layout_static
# 	gvplugin_neato_layout_static
# 	gvplugin_pango_static
# 	gvplugin_gd_static
# 	gvplugin_core_static
# 	pathplan
# 	graph
# 	cdt
# 	gvgd
# 	pango
# 	cairo
# 	glib
# 	fontconfig
# 	freetype
# 	png
# 	jpeg
# 	z
# 	expat
# 	ltdl
# 	m
# )
# 
# INSTALL_TARGETS(/bin dot_static )
# 
# INSTALL_FILES( FILES dot.pdf )
