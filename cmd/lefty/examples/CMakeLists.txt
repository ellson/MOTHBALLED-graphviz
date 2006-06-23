# $Id$ $Revision$
## Process this file with cmake to produce Makefile

########### install files ###############

INSTALL_FILES(FILES
	box.lefty
	def.lefty
	fractal.lefty
	fractal2.lefty
	slides.lefty
	tree.lefty
)



#original Makefile.am contents follow:

## $Id$ $Revision$
### Process this file with automake to produce Makefile.in
#
#leftydir = $(pkgdatadir)/lefty
#
#lefty_DATA = box.lefty def.lefty fractal.lefty fractal2.lefty \
#	slides.lefty tree.lefty
#
#EXTRA_DIST = $(lefty_DATA)
