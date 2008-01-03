# $Id$ $Revision$
## Process this file with automake to produce Makefile.in

pkgindexdir = @TCL_INSTALL_DIR@/@PACKAGE_NAME@-@PACKAGE_VERSION@
pkgindex_DATA = pkgIndex.tcl

SUBDIRS = tclstubs tkstubs tclhandle gdtclft tcldot tclpathplan tkspline gv

pkgIndex.tcl: @TCL_PKGINDEX@ @TCL_PKGINDEX_GD@ @TCL_PKGINDEX_SWIG@ @TK_PKGINDEX@
	echo "# end" >end
	cat @TCL_PKGINDEX@ @TCL_PKGINDEX_GD@ @TCL_PKGINDEX_SWIG@ @TK_PKGINDEX@ end >$@
	rm -f end

EXTRA_DIST = Makefile.old mkpkgindex.sh

DISTCLEANFILES = pkgIndex.tcl
