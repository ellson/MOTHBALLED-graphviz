INCLUDE(FindTCL)

INCLUDE_DIRECTORIES(
	${tcldot_SRCDIR}
	${top_SRCDIR}
	${tclhandle_SRCDIR}
	${gvc_SRCDIR}
	${common_SRCDIR}
	${graph_SRCDIR}
	${cdt_SRCDIR}
	${pathplan_SRCDIR}
	${TCL_INCLUDE_PATH}
)

SET(tcldot_SRCS
	${tcldot_SRCDIR}/tcldot.c
	${tcldot_SRCDIR}/tkgen.c
)
