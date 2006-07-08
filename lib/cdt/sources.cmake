# $Id$ $Revision$

INCLUDE_DIRECTORIES(
	${cdt_SRCDIR}
	${top_SRCDIR}
)

SET(cdt_SRCS
	${cdt_SRCDIR}/dtclose.c
	${cdt_SRCDIR}/dtdisc.c
	${cdt_SRCDIR}/dtextract.c
	${cdt_SRCDIR}/dtflatten.c
	${cdt_SRCDIR}/dthash.c
	${cdt_SRCDIR}/dtlist.c
	${cdt_SRCDIR}/dtmethod.c
	${cdt_SRCDIR}/dtopen.c
	${cdt_SRCDIR}/dtrenew.c
	${cdt_SRCDIR}/dtrestore.c
	${cdt_SRCDIR}/dtsize.c
	${cdt_SRCDIR}/dtstat.c
	${cdt_SRCDIR}/dtstrhash.c
	${cdt_SRCDIR}/dttree.c
	${cdt_SRCDIR}/dtview.c
	${cdt_SRCDIR}/dtwalk.c
)
