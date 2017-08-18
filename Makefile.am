# $Id$ $Revision$
## Process this file with automake to produce Makefile.in

ACLOCAL_AMFLAGS = -I m4

export TAR_OPTIONS = --owner=0 --group=0 --exclude=windows/dependencies/*/*

txtdir = $(pkgdatadir)/doc
htmldir = $(pkgdatadir)/doc/html

txt = AUTHORS COPYING ChangeLog NEWS cpl1.0.txt
html =

txt_DATA = $(txt)
html_DATA = $(html)

pkginclude_HEADERS = $(top_builddir)/graphviz_version.h

man_MANS = graphviz.7

# $(subdirs) contains the list from: AC_CONFIG_SUBDIRS
SUBDIRS = $(subdirs) lib plugin cmd tclpkg doc contrib share graphs rtest tests

.PHONY: doxygen
doxygen:
	doxygen

dist-hook:
#	find $(distdir) -name CVS -type d -prune -exec rm -rf {} \;
#	rm -f ToPAS
#	find $(distdir) -type f -exec md5sum {} >>ToPAS \;

BUILT_SOURCES = $(top_builddir)/graphviz_version.h

$(top_builddir)/graphviz_version.h: config.h
	$(EGREP) 'PACKAGE|VERSION|GVPLUGIN' config.h > $(top_builddir)/graphviz_version.h

EXTRA_DIST = $(txt) $(html) graphviz.spec \
	autogen.sh config/depcomp config/config.rpath \
	builddate.h compat.h \
	Makeargs iffe config.iffe \
	m4/README m4/iconv.m4 m4/lib-ld.m4 m4/lib-link.m4 m4/lib-prefix.m4 \
	graphviz.7 Doxyfile.in Doxyfile awk \
	config/config_perl.pl config/config_python.py \
	config/config_ruby.rb config/config_tcl.tcl dot.demo  plugin.demo \
	macosx windows debian redhat graphviz.sln

.PHONY: rpm
rpm: dist
	mkdir -p $(HOME)/rpmbuild/$(HOSTNAME)/SPECS $(HOME)/rpmbuild/$(HOSTNAME)/SOURCES
	cp graphviz-@VERSION@.tar.gz $(HOME)/rpmbuild/$(HOSTNAME)/SOURCES/
	case `cut -d' ' -f1 /etc/system-release` in \
	Fedora) cp redhat/graphviz.spec.fedora $(HOME)/rpmbuild/$(HOSTNAME)/SPECS/graphviz.spec;; \
	CentOS) cp redhat/graphviz.spec.rhel $(HOME)/rpmbuild/$(HOSTNAME)/SPECS/graphviz.spec;; \
	Red) cp redhat/graphviz.spec.rhel $(HOME)/rpmbuild/$(HOSTNAME)/SPECS/graphviz.spec;; \
	esac
	rpmbuild -ba $(HOME)/rpmbuild/$(HOSTNAME)/SPECS/graphviz.spec
#	rpmbuild -ta graphviz-@VERSION@.tar.gz
#	rpmbuild -ta --define "rtest 1" graphviz-@VERSION@.tar.gz
	
.PHONY: deb
deb: dist
	rm -rf graphviz-@VERSION@
	zcat graphviz-@VERSION@.tar.gz | tar xf -
	(cd graphviz-@VERSION@; fakeroot make -f debian/rules binary)
	
.PHONY: pkg
pkg: dist
	tar xzf graphviz-@VERSION@.tar.gz && make -C graphviz-@VERSION@/macosx/build && mv graphviz-@VERSION@/macosx/build/graphviz-*.pkg .

# codespell is a python script:
#      https://github.com/lucasdemarchi/codespell
# install on Fedora26 with:
#      pip install codespell
.PHONY: spellcheck
spellcheck:
	codespell -d -q 3 --skip="*.po,*.ts"

DISTCLEANFILES = config.cache builddate.h FEATURE/* graphviz-@VERSION@*
