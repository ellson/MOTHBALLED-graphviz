# $Id$ $Revision$
## Process this file with cmake to produce Makefile

ADD_SUBDIRECTORY( pathplan_data ) 

########### install files ###############

INSTALL_FILES(FILES
	pathplan
)
INSTALL_FILES(FILES
	pathplan.README
)



#original Makefile.am contents follow:

## $Id$ $Revision$
### Process this file with automake to produce Makefile.in
#
#SUBDIRS = pathplan_data
#
#demodir = $(pkgdatadir)/demo
#
#if WITH_TCL
#demo_SCRIPTS = pathplan
#demo_DATA = pathplan.README
#endif
#
#EXTRA_DIST = pathplan pathplan.README
