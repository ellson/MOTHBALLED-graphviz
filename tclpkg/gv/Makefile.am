## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = \
        -I$(top_srcdir) \
	-I$(top_srcdir)/lib/gvc \
	-I$(top_srcdir)/lib/common \
	-I$(top_srcdir)/lib/graph \
	-I$(top_srcdir)/lib/cdt \
	-I$(top_srcdir)/lib/pathplan @PERL_INCLUDES@ @PYTHON_INCLUDES@ @RUBY_INCLUDES@ @TCL_INCLUDES@ 

LIBS =  @LIBS@ -lc

if DISABLE_GVRENDER
BASELIBS = \
        $(top_builddir)/lib/common/libcommon.la \
        $(top_builddir)/lib/gvc/libgvc.la @LIBGEN_LIBS@
else
BASELIBS = \
        $(top_builddir)/lib/common/libcommon.la \
	$(top_builddir)/lib/gvc/libgvc.la \
        $(top_builddir)/plugin/cairo/libgvplugin_cairo.la \
        $(top_builddir)/plugin/gd/libgvplugin_gd.la \
        $(top_builddir)/plugin/text/libgvplugin_text.la \
        $(top_builddir)/plugin/layout/libgvplugin_layout.la @CAIRO_LIBS@ @LIBGEN_LIBS@
endif

noinst_HEADERS = gv.h

pkglib_LTLIBRARIES = 
pkglib_DATA =
if WITH_SWIG
if WITH_PERL
pkglib_LTLIBRARIES += libgv_perl.la
pkglib_DATA += gv.pm
endif
if WITH_PYTHON
pkglib_LTLIBRARIES += libgv_python.la
pkglib_DATA += gv.py
endif
if WITH_RUBY
pkglib_LTLIBRARIES += libgv_ruby.la
endif
if WITH_TCL
pkglib_LTLIBRARIES += libgv_tcl.la
endif
endif

libgv_perl_la_SOURCES = gv.cpp gv_perl.cpp plugins.c 
libgv_perl_la_LIBADD = $(BASELIBS) @PERL_LIBS@

libgv_python_la_SOURCES = gv.cpp gv_python.cpp plugins.c 
libgv_python_la_LIBADD = $(BASELIBS) @PYTHON_LIBS@

libgv_ruby_la_SOURCES = gv.cpp gv_ruby.cpp plugins.c 
libgv_ruby_la_LIBADD = $(BASELIBS) @RUBY_LIBS@

libgv_tcl_la_SOURCES = gv.cpp gv_tcl.cpp plugins.c 
libgv_tcl_la_LIBADD = $(BASELIBS) @TCL_LIBS@

gv_perl.cpp gv.pm: gv.i
	$(SWIG) -c++ -perl -o gv_perl.cpp gv.i

gv_python.cpp gv.py: gv.i
	$(SWIG) -c++ -python -o gv_python.cpp gv.i

gv_ruby.cpp: gv.i
	$(SWIG) -c++ -ruby -o gv_ruby.cpp gv.i

gv_tcl.cpp: gv.i
	$(SWIG) -c++ -tcl -namespace -o gv_tcl.cpp gv.i

test: test_perl test_python test_ruby test_tcl

test_perl:
	mkdir -p perl
	(cd perl; \
		ln -fs ../.libs/libgv_perl.so gv.so; \
		ln -fs ../gv.pm gv.pm; \
		ln -fs ../hello.dot hello.dot; \
		ln -fs ../test.pl test.pl; \
		./test.pl)

test_python:
	mkdir -p python
	(cd python; \
		ln -fs ../.libs/libgv_python.so _gv.so; \
		ln -fs ../gv.py gv.py; \
		ln -fs ../hello.dot hello.dot; \
		ln -fs ../test.py test.py; \
		PYTHONPATH=. ./test.py)

test_ruby:
	mkdir -p ruby
	(cd ruby; \
		ln -fs ../.libs/libgv_ruby.so gv.so; \
		ln -fs ../hello.dot hello.dot; \
		ln -fs ../test.rb test.rb; \
		./test.rb)

test_tcl:
	mkdir -p tcl
	(cd tcl; \
		ln -fs ../.libs/libgv_tcl.so gv.so; \
		ln -fs ../hello.dot hello.dot; \
		ln -fs ../test.tcl test.tcl; \
		./test.tcl)

install-exec-hook:

EXTRA_DIST = gv.i

CLEANFILES = gv_perl.cpp gv.pm gv_python.cpp gv.py gv.pyc gv_ruby.cpp gv_tcl.cpp \
	perl/* python/* ruby/* tcl/*

DISTCLEANFILES =
