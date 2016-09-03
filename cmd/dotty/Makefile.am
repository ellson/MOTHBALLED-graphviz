# $Id$ $Revision$
## Process this file with automake to produce Makefile.in

leftydir = $(pkgdatadir)/lefty
pdfdir = $(pkgdatadir)/doc/pdf

if WITH_X
if WITH_WIN32
bin_PROGRAMS = dotty
else
bin_SCRIPTS = dotty
endif
lefty_DATA = dotty.lefty dotty_draw.lefty dotty_edit.lefty \
        dotty_layout.lefty dotty_ui.lefty
endif

man_MANS = dotty.1
pdf_DATA = dotty.1.pdf

if WITH_WIN32
dotty_SOURCES = mswin32/dotty.c
dotty_LDADD = -luser32 -lgdi32 -lwinspool -lcomdlg32 -ladvapi32 -lshell32
else
dotty: $(top_srcdir)/cmd/dotty/dotty.sh
	cp $(top_srcdir)/cmd/dotty/dotty.sh dotty
	chmod +x dotty
endif

dotty.1.pdf: $(srcdir)/dotty.1
	- @GROFF@ -Tps -man $(srcdir)/dotty.1 | @PS2PDF@ - - >dotty.1.pdf

EXTRA_DIST = $(lefty_DATA) $(man_MANS) $(pdf_DATA) \
	dotty.bsh dotty.ksh dotty.sh \
	notes mswin32 dotty.vcxproj*

DISTCLEANFILES = $(pdf_DATA) dotty
