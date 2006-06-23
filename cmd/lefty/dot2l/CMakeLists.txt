# $Id$ $Revision$
## Process this file with cmake to produce Makefile

INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}/cmd/lefty
)


########### next target ###############

ADD_CUSTOM_COMMAND(
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	OUTPUT dotparse.c
	OUTPUT dotparse.h
	DEPENDS dotparse.y
	COMMAND yacc -dv dotparse.y
	COMMAND cat < y.tab.c > dotparse.c
	COMMAND cat < y.tab.h > dotparse.h
)

SET(dot2l_SRCS
	dotparse.h
	dot2l.c
	dotlex.c
	dotparse.c
	dottrie.c
)

ADD_LIBRARY(dot2l STATIC ${dot2l_SRCS})


########### install files ###############




#original Makefile.am contents follow:

## $Id$ $Revision$
### Process this file with automake to produce Makefile.in
#
#AM_CPPFLAGS = -I$(top_srcdir)/cmd/lefty
#
#AM_YFLAGS = -dv
#
#noinst_HEADERS = dot2l.h triefa.h dotparse.h
#if WITH_X
#noinst_LTLIBRARIES = libdot2l.la
#endif
#
#libdot2l_la_SOURCES = dot2l.c dotlex.c dotparse.y dottrie.c 
#
#dotlex.o dotlex.lo dottrie.o dottrie.lo: dotparse.h dotparse.c
#
#dotparse.c : y.output
#	cat < y.tab.c > dotparse.c
#
#dotparse.h : y.output
#	cat < y.tab.h > dotparse.h
#
#y.output: $(top_srcdir)/cmd/lefty/dot2l/dotparse.y
#	@YACC@ -dv $(top_srcdir)/cmd/lefty/dot2l/dotparse.y
#
## this is a home-made tool
## trie.c : trie_input
##       triegen  < trie_input > trie.c
#
#DISTCLEANFILES = y.output y.tab.[ch] dotparse.[ch]
#
#EXTRA_DIST = $(libdot2l_la_SOURCES) triefa.c \
#	y.tab.c y.tab.h y.output
