# $Id$ $Revision$
## Process this file with automake to produce Makefile.in

pkgindexdir = $(libdir)/@PACKAGE@
pkgindex_DATA = pkgIndex.tcl

SUBDIRS = tclstubs tkstubs tclhandle gdtclft tcldot tclpathplan tkspline gv

pkgIndex.tcl: @TCL_PKGINDEX@ @TK_PKGINDEX@
	(cat @TCL_PKGINDEX@ @TK_PKGINDEX@ ; echo "# end") >$@

EXTRA_DIST = Makefile.old mkpkgindex.sh

DISTCLEANFILES = pkgIndex.tcl
