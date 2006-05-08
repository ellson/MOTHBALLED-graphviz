# $Id$ $Revision$
## Process this file with automake to produce Makefile.in

pdfdir = $(pkgdatadir)/doc/pdf
pkgconfigdir = $(libdir)/pkgconfig

man = graph.3
pdf = graph.pdf

AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/lib/cdt

pkginclude_HEADERS = graph.h libgraph.h triefa.h parser.h agxbuf.h
pkglib_LTLIBRARIES = libgraph.la
pkgconfig_DATA = libgraph.pc
man_MANS = $(man)
pdf_DATA = $(pdf)

libgraph_la_LDFLAGS = -version-info @VERSION_INFO@ -no-undefined

libgraph_la_SOURCES = attribs.c agxbuf.c edge.c graph.c graphio.c lexer.c \
	node.c parser.y refstr.c trie.c
 
libgraph_la_LIBADD = $(top_builddir)/lib/cdt/libcdt.la

lexer.o lexer.lo trie.o trie.lo: parser.h

parser.c: y.output
	@SED@ "s/yy/ag/g" < y.tab.c > parser.c

parser.h: y.output
	@SED@ "s/yy/ag/g" < y.tab.h > parser.h

y.output: $(top_srcdir)/lib/graph/parser.y
	@YACC@ -dv $(top_srcdir)/lib/graph/parser.y

# this is a home-made tool
# trie.c : trie_input
#       triegen  < trie_input > trie.c

.3.pdf:
	groff -Tps -man $< | ps2pdf - - > $@

EXTRA_DIST = $(man) $(pdf) triefa.cP \
	Makefile.old parser.c parser.h \
	y.tab.c y.tab.h y.output

DISTCLEANFILES = y.output parser.c parser.h y.tab.[ch] $(pdf)
