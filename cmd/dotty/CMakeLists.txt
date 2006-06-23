# $Id$ $Revision$
## Process this file with cmake to produce Makefile

########### install files ###############

INSTALL_FILES( FILES
	dotty.lefty 
	dotty_draw.lefty 
	dotty_edit.lefty
	dotty_layout.lefty
	dotty_ui.lefty
)
INSTALL_FILES( FILES
	dotty.pdf
)
INSTALL_FILES( FILES
	dotty.1
)



#original Makefile.am contents follow:

## $Id$ $Revision$
### Process this file with automake to produce Makefile.in
#
#man = dotty.1
#pdf = dotty.pdf
#
#leftydir = $(pkgdatadir)/lefty
#pdfdir = $(pkgdatadir)/doc/pdf
#
#man_MANS = $(man)
#pdf_DATA = $(pdf)
#bin_SCRIPTS = dotty
#lefty_DATA = dotty.lefty dotty_draw.lefty dotty_edit.lefty \
#        dotty_layout.lefty dotty_ui.lefty
#
#dotty: $(top_srcdir)/cmd/dotty/dotty.sh
#	cp $(top_srcdir)/cmd/dotty/dotty.sh dotty
#	chmod +x dotty
#
#.1.pdf:
#	groff -Tps -man $< | ps2pdf - - >$@
#
#EXTRA_DIST = $(lefty_DATA) $(man) $(pdf) \
#	dotty.bsh dotty.ksh dotty.sh \
#	Makefile.old notes mswin32
#
#DISTCLEANFILES = $(pdf) dotty
