## Process this file with automake to produce Makefile.in

DOTTYSCRIPTS = dotty.lefty dotty_draw.lefty dotty_edit.lefty \
	dotty_layout.lefty dotty_ui.lefty

man = dotty.1
pdf = dotty.pdf

leftydir = $(pkgdatadir)/lefty
pdfdir = $(pkgdatadir)/doc/pdf

man_MANS = $(man)
pdf_DATA = $(pdf)
bin_SCRIPTS = dotty
lefty_SCRIPTS = $(DOTTYSCRIPTS)

dotty: $(top_srcdir)/cmd/dotty/dotty.sh
	cp $(top_srcdir)/cmd/dotty/dotty.sh dotty
	chmod +x dotty

.1.pdf:
	groff -Tps -man $< | ps2pdf - - >$@

EXTRA_DIST = $(DOTTYSCRIPTS) $(man) $(pdf) \
	dotty.bsh dotty.ksh dotty.sh \
	Makefile.old notes mswin32

DISTCLEANFILES = $(pdf) dotty
