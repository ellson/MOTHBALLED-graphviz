## Process this file with automake to produce Makefile.in

SUBDIRS = ws os cs2l dot2l examples

AM_CPPFLAGS = \
	-I$(top_srcdir)/cmd/lefty/ws/x11 \
	-I$(top_srcdir)/cmd/lefty/os/unix \
	-I$(top_srcdir)/cmd/lefty/dot2l @XAW_INCLUDES@

man = lefty.1
pdf = lefty.pdf

leftydir = $(pkgdatadir)/lefty
pdfdir = $(pkgdatadir)/doc/pdf

AM_CFLAGS = -DLEFTYPATH=\"$(leftydir)\" -DHAVEDOT @X_CFLAGS@

noinst_HEADERS = code.h common.h display.h exec.h g.h gfxview.h internal.h \
	io.h lex.h mem.h parse.h str.h tbl.h txtview.h
if WITH_X
bin_PROGRAMS = lefty
man_MANS = $(man)
pdf_DATA = $(pdf)
endif

lefty_SCRIPTS = lefty.psp

lefty_SOURCES = code.c common.c display.c exec.c g.c gfxview.c internal.c \
	lefty.c lex.c mem.c parse.c str.c tbl.c txtview.c

#
# NB. -lXaw must be before -lXt or strange inability-to-get-mouse-focus
# problems can occur on some systems.
#
lefty_LDADD = $(top_builddir)/lefty/dot2l/libdot2l.la \
	$(top_builddir)/lefty/ws/x11/libws.la \
	$(top_builddir)/lefty/ws/x11/libfilereq/libfilereq.la \
	$(top_builddir)/lefty/os/unix/libos.la \
	@X_LIBS@ @XAW_LIBS@ @X_PRE_LIBS@ @XPM_LIBS@ \
	-lXt -lXmu -lXext -lX11 @SOCKET_LIBS@ @MATH_LIBS@

.1.pdf:
	groff -Tps -man $< | ps2pdf - - >$@

EXTRA_DIST = $(lefty_SCRIPTS) $(lefty_SOURCES) $(man) $(pdf) \
	colors.txt Makefile.old aix_mods

DISTCLEANFILES = $(pdf)
