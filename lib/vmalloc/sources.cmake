# $Id$ $Revision$

INCLUDE_DIRECTORIES(
	${vmalloc_SRCDIR}
	${top_SRCDIR}
	${ast_SRCDIR}
	${sfio_SRCDIR}
)

SET(vmalloc_SRCS
	${vmalloc_SRCDIR}/malloc.c
	${vmalloc_SRCDIR}/vmbest.c
	${vmalloc_SRCDIR}/vmclear.c
	${vmalloc_SRCDIR}/vmclose.c
	${vmalloc_SRCDIR}/vmdcheap.c
	${vmalloc_SRCDIR}/vmdebug.c
	${vmalloc_SRCDIR}/vmdisc.c
	${vmalloc_SRCDIR}/vmexit.c
	${vmalloc_SRCDIR}/vmlast.c
	${vmalloc_SRCDIR}/vmopen.c
	${vmalloc_SRCDIR}/vmpool.c
	${vmalloc_SRCDIR}/vmprivate.c
	${vmalloc_SRCDIR}/vmprofile.c
	${vmalloc_SRCDIR}/vmregion.c
	${vmalloc_SRCDIR}/vmsegment.c
	${vmalloc_SRCDIR}/vmset.c
	${vmalloc_SRCDIR}/vmstat.c
	${vmalloc_SRCDIR}/vmstrdup.c
	${vmalloc_SRCDIR}/vmtrace.c
	${vmalloc_SRCDIR}/vmwalk.c
)
