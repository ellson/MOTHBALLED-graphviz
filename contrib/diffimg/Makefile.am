# $Id$ $Revision$
## Process this file with automake to produce Makefile.in

VERSION=0.2

AM_CPPFLAGS = @GD_INCLUDES@

if WITH_LIBGD
noinst_PROGRAMS = diffimg
endif

diffimg_SOURCES = diffimg.c

if WITH_LIBGD
diffimg_LDADD = $(top_builddir)/lib/gd/libgvgd_C.la \
	@FC_LIBS@ @FT_LIBS@ @JPEG_LIBS@ @PNG_LIBS@ @Z_LIBS@ @MATH_LIBS@
else
diffimg_LDADD = @GD_LIBS@
endif

GRAPH = "digraph G { hello -> world }"

test: diffimg
	echo $(GRAPH) | dot -Tpng:cg >hello1.png
	echo $(GRAPH) | dot -Tpng:cg >hello2.png
	if `./diffimg hello1.png hello2.png >test1.png`;then echo same;else echo different;fi
	echo $(GRAPH) | dot -Grankdir=LR -Tpng:cg >hello2.png
	if `./diffimg hello1.png hello2.png >test2.png`;then echo same;else echo different;fi

CLEANFILES = *.png

