#!/bin/bash

VERSION=`ls graphviz-*.tar.gz | sed -e "s/graphviz-//" -e "s/.tar.gz//"`
BUILDDATE=`date -u`

rm -rf graphviz-win graphviz-win.zip graphviz-win.tgz
tar xfzo graphviz-$VERSION.tar.gz
mv graphviz-$VERSION graphviz-win

cd graphviz-win

# Clean out parts not needed for Windows makes
rm -rf \
	config \
	config.h \
	config.h.in \
	config.h.old \
	config.log \
	config.status \
	configure \
	configure.in \
	configure.ac \
	configure.old \
	Config.mk \
	features \
	m4 \
	aclocal.m4 \
	autogen.sh \
	depcomp \
	graphviz.spec.in \
	graphviz.spec \
	iffe \
	libtool \
	mac \
	makearch \
	stamp-h1 \
	rtest \
	INSTALL.old \
	graphviz-win.sh

find . -name "Make*" -exec rm -f {} \;

# for now, lets try to fix these to be portable in the main tree
# leaving: windows/config.h windows/cdt/ast_common.h
rm -f \
	windows/lib/agraph/grammar.c \
	windows/lib/agraph/grammar.h \
	windows/lib/agraph/scan.c \
	windows/lib/graph/parser.c \
	windows/lib/graph/parser.h \
	windows/lib/common/colortbl.h \
	windows/cmd/lefty/dot2l/dotparse.c \
	windows/cmd/lefty/dot2l/dotparse.h

# Merge all the useful bits onto the main tree, then lose the windows dir
cp -rp windows/* .
rm -rf windows

# Provide a VERSION and BUILDDATE
( head -16 config.h
  echo "#define VERSION \"$VERSION\""
  echo "#define BUILDDATE \"$BUILDDATE\""
  tail +17 config.h ) > t
mv t config.h

# package the result
cd ..
#zip -rq graphviz-win graphviz-win
# rm -rf graphviz-win
tar cf - graphviz-win | gzip >graphviz-win.tgz
