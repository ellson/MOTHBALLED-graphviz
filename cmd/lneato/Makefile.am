## Process this file with automake to produce Makefile.in

pdfdir = $(pkgdatadir)/doc/pdf

man = lneato.1
pdf = lneato.pdf

man_MANS = $(man)
pdf_DATA = $(pdf)
bin_SCRIPTS = lneato

lneato: ${top_srcdir}/cmd/lneato/lneato.sh
	cp ${top_srcdir}/cmd/lneato/lneato.sh lneato
	chmod +x lneato

.1.pdf:
	groff -Tps -man $< | ps2pdf - - >$@

EXTRA_DIST = $(man) $(pdf) lneato.bsh lneato.ksh lneato.sh \
	Makefile.old mswin32 

DISTCLEANFILES = $(bin_SCRIPTS) $(pdf)
