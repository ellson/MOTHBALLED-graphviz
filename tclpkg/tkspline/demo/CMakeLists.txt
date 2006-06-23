# $Id$ $Revision$
## Process this file with cmake to produce Makefile

########### install files ###############

INSTALL_FILES(FILES
	spline
)
INSTALL_FILES(FILES
	spline.README
)


#original Makefile.am contents follow:

## $Id$ $Revision$
### Process this file with automake to produce Makefile.in
#
#demodir = $(pkgdatadir)/demo
#
#if WITH_TK
#demo_SCRIPTS = spline
#demo_DATA = spline.README
#endif
#
#EXTRA_DIST = spline spline.README
