## Process this file with automake to produce Makefile.in

pdfdir = $(pkgdatadir)/doc/pdf
pkgconfigdir = $(pkglibdir)/pkgconfig

linkedman = neato.1 twopi.1 fdp.1 circo.1
linkedpdf = neato.pdf twopi.pdf fdp.pdf circo.pdf
linkedprogram = neato twopi fdp circo

AM_CPPFLAGS = \
        -I$(top_srcdir) \
        -I$(top_srcdir)/lib/common \
        -I$(top_srcdir)/lib/gvc \
	-I$(top_srcdir)/lib/dotgen \
	-I$(top_srcdir)/lib/neatogen \
	-I$(top_srcdir)/lib/twopigen \
	-I$(top_srcdir)/lib/fdpgen \
	-I$(top_srcdir)/lib/circogen \
	-I$(top_srcdir)/lib/pack \
	-I$(top_srcdir)/lib/pathplan \
	-I$(top_srcdir)/lib/graph \
	-I$(top_srcdir)/lib/cdt

pkginclude_HEADERS = dotneato.h
pkglib_LTLIBRARIES = libdotneato.la
# pkgconfig_DATA = libdotneato.pc
bin_SCRIPTS = dotneato-config
bin_PROGRAMS = dot
man_MANS = dot.1 dotneato-config.1
pdf_DATA = dot.pdf dotneato-config.pdf

libdotneato_la_SOURCES = dotneato.c
libdotneato_la_LIBADD = \
	$(top_builddir)/lib/common/libcommon.la \
	$(top_builddir)/lib/gvc/libgvc.la

dot_SOURCES = dot.c args.c plugins.c

if DISABLE_GVRENDER
dot_LDADD = \
        $(top_builddir)/lib/common/libcommon.la \
        $(top_builddir)/lib/gvc/libgvc.la \
	$(top_builddir)/plugin/layout/libgvplugin_layout.la @LIBGEN_LIBS@
else
dot_LDADD = \
	$(top_builddir)/lib/common/libcommon.la \
        $(top_builddir)/lib/gvc/libgvc.la \
        $(top_builddir)/plugin/cairo/libgvplugin_cairo.la \
        $(top_builddir)/plugin/gd/libgvplugin_gd.la \
        $(top_builddir)/plugin/text/libgvplugin_text.la \
        $(top_builddir)/plugin/layout/libgvplugin_layout.la @CAIRO_LIBS@ @LIBGEN_LIBS@
endif

install-data-hook:
	(cd $(DESTDIR)$(man1dir); for i in $(linkedman); do rm -f $$i; $(LN_S) dot.1 $$i; done;)
	(cd $(DESTDIR)$(pdfdir); for i in $(linkedpdf); do rm -f $$i; $(LN_S) dot.pdf $$i; done;)

install-exec-hook:
	(cd $(DESTDIR)$(bindir); for i in $(linkedprogram); do rm -f $$i; $(LN_S) dot $$i; done;)

uninstall-hook:
	(cd $(DESTDIR)$(man1dir); for i in $(linkedman); do rm -f $$i; done;)
	(cd $(DESTDIR)$(pdfdir); for i in $(linkedpdf); do rm -f $$i; done;)
	(cd $(DESTDIR)$(bindir); for i in $(linkedprogram); do rm -f $$i; done;)

dot.pdf: $(top_srcdir)/cmd/dot/dot.1
	groff -Tps -man $(top_srcdir)/cmd/dot/dot.1 | ps2pdf - - >$@

dotneato-config.pdf: $(top_srcdir)/cmd/dot/dotneato-config.1
	groff -Tps -man $(top_srcdir)/cmd/dot/dotneato-config.1 | ps2pdf - - >$@

EXTRA_DIST = Makefile.old dot.1 dotneato-config.1 dot.pdf dotneato-config.pdf dotneato-config.in

DISTCLEANFILES = dot.pdf dotneato-config.pdf
