# $Id$ $Revision$
## Process this file with automake to produce Makefile.in

VERSION=0.2

AM_CPPFLAGS = @GD_INCLUDES@

if WITH_LIBGD
noinst_PROGRAMS = diffimg
endif

diffimg_SOURCES = diffimg.c

diffimg_LDADD = @GD_LIBS@

GRAPH = "digraph G { hello -> world }"

test: diffimg
	echo $(GRAPH) | dot -Tpng:cg >hello1.png
	echo $(GRAPH) | dot -Tpng:cg >hello2.png
	if `./diffimg hello1.png hello2.png >test1.png`;then echo same;else echo different;fi
	echo $(GRAPH) | dot -Grankdir=LR -Tpng:cg >hello2.png
	if `./diffimg hello1.png hello2.png >test2.png`;then echo same;else echo different;fi

CLEANFILES = *.png

