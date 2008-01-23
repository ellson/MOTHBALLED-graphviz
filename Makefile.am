# $Id$ $Revision$
## Process this file with automake to produce Makefile.in

ACLOCAL_AMFLAGS = -I m4

txtdir = $(pkgdatadir)/doc
htmldir = $(pkgdatadir)/doc/html

txt = AUTHORS COPYING ChangeLog NEWS cpl1.0.txt
html =

txt_DATA = $(txt)
html_DATA = $(html)

man_MANS = graphviz.7

# $(subdirs) contains the list from: AC_CONFIG_SUBDIRS
SUBDIRS = $(subdirs) lib plugin cmd tclpkg doc contrib share graphs rtest

.PHONY: doxygen
doxygen:
	doxygen

dist-hook:
	find $(distdir) -name CVS -type d -prune -exec rm -rf {} \;
#	rm -f ToPAS
#	find $(distdir) -type f -exec md5sum {} >>ToPAS \;

EXTRA_DIST = $(txt) $(html) graphviz.spec.in graphviz.spec \
	autogen.sh config/depcomp config/config.rpath \
	builddate.h ast_common.h.in compat.h \
	Makefile.old Config.mk.old configure.old INSTALL.old \
	Makeargs config.h.old compat_getopt.h iffe config.iffe \
	m4/README m4/iconv.m4 m4/lib-ld.m4 m4/lib-link.m4 m4/lib-prefix.m4 \
	graphviz.7 Doxyfile.in Doxyfile \
	makearch features awk windows \
	config/config_perl.pl config/config_python.py \
	config/config_ruby.rb config/config_tcl.tcl dot.demo \
	macbuild macosx

.PHONY: rpm
rpm: dist
	rpmbuild -ta graphviz-@VERSION@.tar.gz

DISTCLEANFILES = config.cache builddate.h ast_common.h FEATURE/* graphviz-@VERSION@*
