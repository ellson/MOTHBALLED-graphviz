## Process this file with automake to produce Makefile.in

pkgconfigdir = $(libdir)/pkgconfig

AM_CPPFLAGS = \
	-I$(top_srcdir) \
        -I$(top_srcdir)/lib/common \
        -I$(top_srcdir)/lib/gvc \
	-I$(top_srcdir)/lib/graph \
	-I$(top_srcdir)/lib/cdt \
	-I$(top_srcdir)/lib/pathplan

noinst_HEADERS = dot.h dotprocs.h
noinst_LTLIBRARIES = libdotgen.la

libdotgen_la_SOURCES = acyclic.c class1.c class2.c cluster.c compound.c \
	conc.c decomp.c fastgr.c flat.c dotinit.c mincross.c \
	position.c rank.c sameport.c dotsplines.c

EXTRA_DIST = Makefile.old
