# $Id$ $Revision$
## Process this file with automake to produce Makefile.in

pdfdir = $(pkgdatadir)/doc/pdf

linkedman = neato.1 twopi.1 fdp.1 circo.1
linkedpdf = neato.pdf twopi.pdf fdp.pdf circo.pdf
linkedprogram = neato twopi fdp circo

AM_CPPFLAGS = \
        -I$(top_srcdir)/lib/common \
        -I$(top_srcdir)/lib/gvc \
	-I$(top_srcdir)/lib/pathplan \
	-I$(top_srcdir)/lib/graph \
	-I$(top_srcdir)/lib/cdt

if ENABLE_STATIC
bin_PROGRAMS = dot dot_static
else
bin_PROGRAMS = dot
endif
man_MANS = dot.1 
pdf_DATA = dot.pdf

dot_SOURCES = dot.c
dot_LDADD = $(top_builddir)/lib/gvc/libgvc.la

install-data-hook:
	(cd $(DESTDIR)$(man1dir); for i in $(linkedman); do rm -f $$i; $(LN_S) dot.1 $$i; done;)
	(cd $(DESTDIR)$(pdfdir); for i in $(linkedpdf); do rm -f $$i; $(LN_S) dot.pdf $$i; done;)

# create "neato", "fdp" etc as soflinks to "dot"
# run "dot -c", if possible, to create plugin config
install-exec-hook:
	(cd $(DESTDIR)$(bindir); for i in $(linkedprogram); do rm -f $$i; $(LN_S) dot $$i; done;)
	if test "x$(DESTDIR)" = "x"; then (cd $(bindir); ./dot -c;); fi

uninstall-hook:
	(cd $(DESTDIR)$(man1dir); for i in $(linkedman); do rm -f $$i; done;)
	(cd $(DESTDIR)$(pdfdir); for i in $(linkedpdf); do rm -f $$i; done;)
	(cd $(DESTDIR)$(bindir); for i in $(linkedprogram); do rm -f $$i; done;)
	(cd $(DESTDIR)$(libdir); rm -f config;)

dot.pdf: $(top_srcdir)/cmd/dot/dot.1
	groff -Tps -man $(top_srcdir)/cmd/dot/dot.1 | ps2pdf - - >$@

dot_static_SOURCES = dot.c
dot_static_LDFLAGS = -export-dynamic -dlpreopen self -export-symbols-regex '.*_LTX_library'
dot_static_LDADD = \
	$(top_builddir)/lib/plugin/.libs/libgvplugin_dot_layout.a \
	$(top_builddir)/lib/plugin/.libs/libgvplugin_neato_layout.a \
	$(top_builddir)/lib/plugin/.libs/libgvplugin_usershape_gd.a \
	$(top_builddir)/lib/gvc/.libs/libgvc_builtins.a \
	$(top_builddir)/lib/pathplan/.libs/libpathplan.a \
	$(top_builddir)/lib/pack/.libs/libpack.a \
	$(top_builddir)/lib/graph/.libs/libgraph.a \
	$(top_builddir)/lib/cdt/.libs/libcdt.a \
	$(top_builddir)/lib/gd/.libs/libgvgd.a \
	@ICONV_LIBS@ @FC_LIBS@ @FT_LIBS@ @JPEG_LIBS@ @PNG_LIBS@ @MATH_LIBS@ @EXPAT_LIBS@ @Z_LIBS@ @LIBGEN_LIBS@ @LIBLTDL@ @SOCKET_LIBS@

EXTRA_DIST = Makefile.old dot.1 dot.pdf

DISTCLEANFILES = dot.pdf
