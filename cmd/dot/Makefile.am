# $Id$ $Revision$
## Process this file with automake to produce Makefile.in

pdfdir = $(pkgdatadir)/doc/pdf

linkedman = neato.1 twopi.1 fdp.1 circo.1
linkedpdf = neato.pdf twopi.pdf fdp.pdf circo.pdf
linkedprogram = neato$(EXEEXT) twopi$(EXEEXT) fdp$(EXEEXT) circo$(EXEEXT)

AM_CPPFLAGS = \
        -I$(top_srcdir)/lib/common \
        -I$(top_srcdir)/lib/gvc \
	-I$(top_srcdir)/lib/pathplan \
	-I$(top_srcdir)/lib/graph \
	-I$(top_srcdir)/lib/cdt

if ENABLE_SHARED
if ENABLE_LTDL
if ENABLE_STATIC
bin_PROGRAMS = dot dot_static
else
noinst_PROGRAMS = dot_builtins
bin_PROGRAMS = dot
endif
else
if ENABLE_STATIC
noinst_PROGRAMS = dot_builtins
bin_PROGRAMS = dot_static
else
noinst_PROGRAMS = dot_builtins
endif
endif
else
if ENABLE_STATIC
bin_PROGRAMS = dot_static
endif
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
	(cd $(DESTDIR)$(bindir); for i in $(linkedprogram); do rm -f $$i; $(LN_S) dot$(EXEEXT) $$i; done;)
	if test "x$(DESTDIR)" = "x"; then if test -x $(bindir)/dot$(EXEEXT); then if test -x /sbin/ldconfig; then /sbin/ldconfig 2>/dev/null; fi; $(bindir)/dot$(EXEEXT) -c; else $(bindir)/dot_static$(EXEEXT) -c; fi; fi

uninstall-hook:
	(cd $(DESTDIR)$(man1dir); for i in $(linkedman); do rm -f $$i; done;)
	(cd $(DESTDIR)$(pdfdir); for i in $(linkedpdf); do rm -f $$i; done;)
	(cd $(DESTDIR)$(bindir); for i in $(linkedprogram); do rm -f $$i; done;)
	(cd $(DESTDIR)$(libdir); rm -f config;)

dot.pdf: $(top_srcdir)/cmd/dot/dot.1
	- @GROFF@ -Tps -man $(top_srcdir)/cmd/dot/dot.1 | @PS2PDF@ - - >$@

# This automagic technique for building the list of builtins requires that the
# symbols be in the binary image, but libtool makes it difficult
# to get a library into an image if it doesn't already resolve any symbols!
# Workaround is to link explicity to the .libs/libxxx.a
# A better approach is probably to provide an explicit list of builtins
# like $(top_srcdir)/lib/gvc/dot_builtins.c
dot_static_SOURCES = dot.c $(top_srcdir)/lib/gvc/no_demand_loading.c
dot_static_LDFLAGS = -export-dynamic -dlpreopen self -export-symbols-regex '.*_LTX_library'
dot_static_LDADD = \
	$(top_builddir)/plugin/dot_layout/.libs/libgvplugin_dot_layout_C.a \
	$(top_builddir)/plugin/neato_layout/.libs/libgvplugin_neato_layout_C.a \
	$(top_builddir)/plugin/core/.libs/libgvplugin_core_C.a \
	$(top_builddir)/lib/gvc/libgvc_C.la \
	$(top_builddir)/lib/pathplan/libpathplan_C.la \
	$(top_builddir)/lib/graph/libgraph_C.la \
	$(top_builddir)/lib/cdt/libcdt_C.la \
       	@GTS_LIBS@ @ICONV_LIBS@ @EXPAT_LIBS@ @Z_LIBS@ @LIBGEN_LIBS@ @SOCKET_LIBS@ @IPSEPCOLA_LIBS@ @MATH_LIBS@

dot_builtins_SOURCES = dot.c $(top_srcdir)/lib/gvc/dot_builtins.c $(top_srcdir)/lib/gvc/no_demand_loading.c
dot_builtins_LDADD = \
	$(top_builddir)/plugin/dot_layout/libgvplugin_dot_layout.la \
	$(top_builddir)/plugin/neato_layout/libgvplugin_neato_layout.la \
	$(top_builddir)/plugin/core/libgvplugin_core.la \
	$(top_builddir)/lib/gvc/libgvc_builtins.la \
	$(top_builddir)/lib/pathplan/libpathplan.la \
	$(top_builddir)/lib/graph/libgraph.la \
	$(top_builddir)/lib/cdt/libcdt.la \
       	@GTS_LIBS@ @ICONV_LIBS@ @EXPAT_LIBS@ @Z_LIBS@ @LIBGEN_LIBS@ @SOCKET_LIBS@ @IPSEPCOLA_LIBS@ @MATH_LIBS@

if WITH_LIBGD
if WITH_MYLIBGD
dot_static_LDADD += $(top_builddir)/plugin/gd/.libs/libgvplugin_gd_C.a $(top_builddir)/lib/gd/libgvgd_C.la @FONTCONFIG_LIBS@ @FREETYPE2_LIBS@ @JPEG_LIBS@ @PNG_LIBS@ @Z_LIBS@
dot_builtins_LDADD += $(top_builddir)/plugin/gd/.libs/libgvplugin_gd.la $(top_builddir)/lib/gd/libgvgd_C.la @FONTCONFIG_LIBS@ @FREETYPE2_LIBS@ @JPEG_LIBS@ @PNG_LIBS@ @Z_LIBS@
else
dot_static_LDADD += $(top_builddir)/plugin/gd/.libs/libgvplugin_gd.la @GD_LIBS@ @FONTCONFIG_LIBS@ @FREETYPE2_LIBS@ @JPEG_LIBS@ @PNG_LIBS@ @Z_LIBS@
dot_builtins_LDADD += $(top_builddir)/plugin/gd/.libs/libgvplugin_gd.la @GD_LIBS@ @FONTCONFIG_LIBS@ @FREETYPE2_LIBS@ @JPEG_LIBS@ @PNG_LIBS@ @Z_LIBS@
endif
endif

if WITH_PANGOCAIRO
dot_static_LDADD += $(top_builddir)/plugin/pango/.libs/libgvplugin_pango_C.a @PANGOCAIRO_LIBS@
dot_builtins_LDADD += $(top_builddir)/plugin/pango/libgvplugin_pango.la @PANGOCAIRO_LIBS@
endif

EXTRA_DIST = Makefile.old dot.1 dot.pdf

DISTCLEANFILES = dot.pdf
