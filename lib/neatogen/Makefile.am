## Process this file with automake to produce Makefile.in

pkgconfigdir = $(pkglibdir)/pkgconfig

AM_CPPFLAGS = \
        -I$(top_srcdir) \
        -I$(top_srcdir)/lib/common \
        -I$(top_srcdir)/lib/gvc \
        -I$(top_srcdir)/lib/pack \
        -I$(top_srcdir)/lib/pathplan \
        -I$(top_srcdir)/lib/graph \
        -I$(top_srcdir)/lib/cdt

noinst_HEADERS = adjust.h edges.h geometry.h heap.h hedges.h info.h mem.h \
	neato.h poly.h neatoprocs.h simple.h site.h voronoi.h \
	bfs.h closest.h conjgrad.h defs.h dijkstra.h embed_graph.h kkutils.h \
	matrix_ops.h pca.h stress.h
noinst_LTLIBRARIES = libneatogen.la

libneatogen_la_SOURCES = adjust.c circuit.c edges.c find_ints.c geometry.c \
	heap.c hedges.c info.c neatoinit.c intersect.c legal.c lu.c matinv.c \
	memory.c poly.c printvis.c site.c solve.c neatosplines.c stuff.c \
	voronoi.c stress.c kkutils.c matrix_ops.c embed_graph.c dijkstra.c \
	conjgrad.c pca.c closest.c bfs.c constraint.c

EXTRA_DIST = Makefile.old
