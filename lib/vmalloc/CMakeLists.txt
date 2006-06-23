# $Id$ $Revision$
## Process this file with cmake to produce Makefile

INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}/lib/ast
	${CMAKE_SOURCE_DIR}/lib/sfio
)


########### next target ###############

SET(vmalloc_SRCS
	malloc.c
	vmbest.c
	vmclear.c
	vmclose.c
	vmdcheap.c
	vmdebug.c
	vmdisc.c
	vmexit.c
	vmlast.c
	vmopen.c
	vmpool.c
	vmprivate.c
	vmprofile.c
	vmregion.c
	vmsegment.c
	vmset.c
	vmstat.c
	vmstrdup.c
	vmtrace.c
	vmwalk.c
)

ADD_LIBRARY(vmalloc STATIC ${vmalloc_SRCS})


########### install files ###############




#original Makefile.am contents follow:

## $Id$ $Revision$
### Process this file with automake to produce Makefile.in
#
#AM_CPPFLAGS = -I$(top_srcdir)/lib/ast \
#	-I$(top_srcdir)/lib/sfio
#
#noinst_HEADERS = vmalloc.h vmhdr.h
#noinst_LTLIBRARIES = libvmalloc_C.la
#
#libvmalloc_C_la_SOURCES = malloc.c vmbest.c vmclear.c vmclose.c vmdcheap.c \
#	vmdebug.c vmdisc.c vmexit.c vmlast.c vmopen.c vmpool.c vmprivate.c \
#	vmprofile.c vmregion.c vmsegment.c vmset.c vmstat.c vmstrdup.c \
#	vmtrace.c vmwalk.c
#
#${top_builddir}/FEATURE/vmalloc: ${top_srcdir}/lib/vmalloc/features/vmalloc
#	mkdir -p ${top_builddir}/FEATURE
#	${top_srcdir}/iffe - set cc $(CC) $(CCMODE) $(CXFLAGS) : run ${top_srcdir}/lib/vmalloc/features/vmalloc > $@
#
#$(libvmalloc_C_la_OBJECTS): ${top_builddir}/FEATURE/vmalloc
#
#DISTCLEANFILES = ${top_builddir}/FEATURE/vmalloc
#
#EXTRA_DIST = README Makefile.old features
