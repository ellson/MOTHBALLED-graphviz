## Process this file with automake to produce Makefile.in

pkgconfigdir = $(pkglibdir)/pkgconfig

AM_CPPFLAGS = \
        -I$(top_srcdir) \
        -I$(top_srcdir)/lib/common \
        -I$(top_srcdir)/lib/gvc \
        -I$(top_srcdir)/lib/dotogen \
        -I$(top_srcdir)/lib/neatogen \
        -I$(top_srcdir)/lib/pack \
        -I$(top_srcdir)/lib/pathplan \
        -I$(top_srcdir)/lib/graph \
        -I$(top_srcdir)/lib/cdt

noinst_HEADERS = block.h blockpath.h blocktree.h circo.h \
	circpos.h circular.h deglist.h edgelist.h nodelist.h \
	nodeset.h stack.h
noinst_LTLIBRARIES = libcircogen.la

libcircogen_la_SOURCES = circularinit.c nodelist.c block.c edgelist.c \
	stack.c circular.c deglist.c blocktree.c blockpath.c \
	circpos.c nodeset.c

EXTRA_DIST = Makefile.old
