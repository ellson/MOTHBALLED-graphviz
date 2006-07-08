# $Id$ $Revision$

INCLUDE_DIRECTORIES(
	${gvplugin_gtk_SRCDIR}
	${top_SRCDIR}
	${common_SRCDIR}
	${graph_SRCDIR}
	${cdt_SRCDIR}
	${pathplan_SRCDIR}
	${gvc_SRCDIR}
)

SET(gvplugin_gtk_SRCS
	${gvplugin_gtk_SRCDIR}/gvdevice_gtk.c
	${gvplugin_gtk_SRCDIR}/gvplugin_gtk.c
	${gvplugin_gtk_SRCDIR}/callbacks.c
	${gvplugin_gtk_SRCDIR}/interface.c
	${gvplugin_gtk_SRCDIR}/support.c
)
