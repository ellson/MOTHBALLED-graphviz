## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = -I$(top_srcdir)/cmd/lefty

AM_YFLAGS = -dv

noinst_HEADERS = dot2l.h triefa.h dotparse.h
if WITH_X
noinst_LTLIBRARIES = libdot2l.la
endif

libdot2l_la_SOURCES = dot2l.c dotlex.c dotparse.y dottrie.c 

dotlex.o dotlex.lo : dotparse.h

dotparse.c : dotparse.y

dotparse.h : dotparse.c

# this is a home-made tool
# trie.c : trie_input
#       triegen  < trie_input > trie.c

DISTCLEANFILES = y.output dotparse.[ch]

EXTRA_DIST = $(libdot2l_la_SOURCES) triefa.c
