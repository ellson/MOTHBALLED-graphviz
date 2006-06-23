# $Id$ $Revision$
## Process this file with cmake to produce Makefile

INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_SOURCE_DIR}
)


########### next target ###############

SET(prune_SRCS
	generic_list.c
	prune.c
)

ADD_EXECUTABLE(prune ${prune_SRCS})

#TARGET_LINK_LIBRARIES(prune  ${QT_AND_KDECORE_LIBS} )

INSTALL_TARGETS(/bin prune )


########### install files ###############

INSTALL_FILES( FILES
	prune.pdf
)
INSTALL_FILES( FILES
	prune.1
)



#original Makefile.am contents follow:

## $Id$ $Revision$
### Process this file with automake to produce Makefile.in
#
#pdfdir = $(pkgdatadir)/doc/pdf
#
#AM_CPPFLAGS = \
#        -I$(top_srcdir) \
#        -I$(top_srcdir)/lib/ingraphs \
#	-I$(top_srcdir)/lib/agraph \
#	-I$(top_srcdir)/lib/cdt
#
#bin_PROGRAMS = prune
#noinst_HEADERS = generic_list.h
#man_MANS = prune.1
#pdf_DATA = prune.pdf
#
#prune_SOURCES = generic_list.c prune.c
#
#prune_LDADD = $(top_builddir)/lib/ingraphs/libingraphs_C.la \
#	$(top_builddir)/lib/agraph/libagraph.la \
#	$(top_builddir)/lib/cdt/libcdt.la
#
#.1.pdf:
#	groff -Tps -man $< | ps2pdf - - >$@
#
#EXTRA_DIST = $(man_MANS) $(pdf_DATA) Makefile.old Makefile.orig README test.dot
#
#DISTCLEANFILES = $(pdf_DATA)
