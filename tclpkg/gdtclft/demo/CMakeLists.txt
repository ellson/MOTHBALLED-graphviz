# $Id$ $Revision$
## Process this file with cmake to produce Makefile

########### install files ###############

INSTALL_FILES(FILES
	entities
)
INSTALL_FILES(FILES
	entities.README
	entities.html
)


#original Makefile.am contents follow:

## $Id$ $Revision$
### Process this file with automake to produce Makefile.in
#
#demodir = $(datadir)/@PACKAGE@/demo
#
#if WITH_TCL
#demo_SCRIPTS = entities
#demo_DATA = entities.README entities.html
#endif
#
#EXTRA_DIST = entities entities.README entities.html
