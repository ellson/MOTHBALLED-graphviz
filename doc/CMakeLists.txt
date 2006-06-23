# $Id$ $Revision$
## Process this file with cmake to produce Makefile

SUBDIRS(
	info
) 


########### install files ###############

INSTALL_FILES(FILES
	build.html
	FAQ.html
	index.html
	winbuild.html
	pspdf.png
)
INSTALL_FILES(FILES
	Dot.ref
	latex_suggestions.txt
)
INSTALL_FILES(FILES
	dotguide.pdf
	dottyguide.pdf
	leftyguide.pdf
	neatoguide.pdf
)


#original Makefile.am contents follow:

## $Id$ $Revision$
### Process this file with automake to produce Makefile.in
#
## SUBDIRS = dot neato lefty dotty info
#SUBDIRS = info
#
#pdf = dotguide.pdf dottyguide.pdf leftyguide.pdf neatoguide.pdf
#html = build.html FAQ.html index.html winbuild.html pspdf.png
#txt = Dot.ref latex_suggestions.txt
#
#pdfdir = $(pkgdatadir)/doc/pdf
#htmldir = $(pkgdatadir)/doc/html
#txtdir = $(pkgdatadir)/doc
#
#pdf_DATA = $(pdf)
#html_DATA = $(html)
#txt_DATA = $(txt)
#
#EXTRA_DIST = $(txt) $(pdf) $(html)
