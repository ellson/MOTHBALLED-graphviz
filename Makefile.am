## Process this file with automake to produce Makefile.in

ACLOCAL_AMFLAGS = -I m4

txtdir = $(pkgdatadir)/doc
htmldir = $(pkgdatadir)/doc/html

txt = AUTHORS COPYING ChangeLog NEWS
html = CPL.html

txt_DATA = $(txt)
html_DATA = $(html)

SUBDIRS = libltdl lib plugin cmd tclpkg doc 

doxygen:
	doxygen

dist-hook:
	find $(distdir) -name CVS -type d -prune -exec rm -rf {} \;

EXTRA_DIST = $(txt) $(html) graphviz.spec.in graphviz.spec graphviz.cvt \
	autogen.sh config/depcomp config/config.rpath ast_common.h.in \
	Makefile.old Config.mk configure.old INSTALL.old README.cairo \
	Makeargs config.h.old compat_getopt.h iffe \
	m4/README m4/iconv.m4 m4/lib-ld.m4 m4/lib-link.m4 m4/lib-prefix.m4 \
	makearch dot.demo features windows mac

DISTCLEANFILES = ast_common.h FEATURE/* graphviz-@VERSION@*
