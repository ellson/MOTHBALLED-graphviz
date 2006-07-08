# $Id$ $Revision$

INCLUDE_DIRECTORIES(
	${gvc_SRCDIR}
	${top_SRCDIR}
	${common_SRCDIR}
	${pathplan_SRCDIR}
	${graph_SRCDIR}
	${cdt_SRCDIR}
)

# FIXME
ADD_DEFINITIONS(-DGVLIBDIR="\\"\\"")

SET(gvc_SRCS
	${gvc_SRCDIR}/gvrender.c
	${gvc_SRCDIR}/gvlayout.c
	${gvc_SRCDIR}/gvdevice.c
	${gvc_SRCDIR}/gvloadimage.c
	${gvc_SRCDIR}/gvcontext.c
	${gvc_SRCDIR}/gvjobs.c
	${gvc_SRCDIR}/gvevent.c
	${gvc_SRCDIR}/gvplugin.c
	${gvc_SRCDIR}/gvconfig.c
	${gvc_SRCDIR}/gvtextlayout.c
	${gvc_SRCDIR}/gvusershape.c
	${gvc_SRCDIR}/gvc.c
)
