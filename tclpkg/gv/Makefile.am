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
	$(top_builddir)/lib/gvc/libgvc.la \
	$(top_builddir)/plugin/layout/libgvplugin_layout.la @LIBGEN_LIBS@
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
if WITH_JAVA
pkglib_LTLIBRARIES += libgv_java.la
endif
if WITH_PERL
pkglib_LTLIBRARIES += libgv_perl.la
endif
if WITH_PYTHON
pkglib_LTLIBRARIES += libgv_python.la
endif
if WITH_RUBY
pkglib_LTLIBRARIES += libgv_ruby.la
endif
if WITH_TCL
pkglib_LTLIBRARIES += libgv_tcl.la
endif

libgv_java_la_SOURCES = gv.cpp gv_java.cpp plugins.c 
libgv_java_la_LIBADD = $(BASELIBS) @JAVA_LIBS@

libgv_perl_la_SOURCES = gv.cpp gv_perl.cpp plugins.c 
libgv_perl_la_LIBADD = $(BASELIBS) @PERL_LIBS@

libgv_python_la_SOURCES = gv.cpp gv_python.cpp plugins.c 
libgv_python_la_LIBADD = $(BASELIBS) @PYTHON_LIBS@

libgv_ruby_la_SOURCES = gv.cpp gv_ruby.cpp plugins.c 
libgv_ruby_la_LIBADD = $(BASELIBS) @RUBY_LIBS@

libgv_tcl_la_SOURCES = gv.cpp gv_tcl.cpp plugins.c 
libgv_tcl_la_LIBADD = $(BASELIBS) @TCL_LIBS@

gv_java.cpp: gv.i
	$(SWIG) -c++ -java -o gv_java.cpp gv.i

gv_perl.cpp: gv.i
	$(SWIG) -c++ -perl -o gv_perl.cpp gv.i

gv_python.cpp: gv.i
	$(SWIG) -c++ -python -o gv_python.cpp gv.i

gv_ruby.cpp: gv.i
	$(SWIG) -c++ -ruby -o gv_ruby.cpp gv.i

gv_tcl.cpp: gv.i
	$(SWIG) -c++ -tcl -namespace -o gv_tcl.cpp gv.i

install-exec-hook:
	cp $(top_srcdir)/tclpkg/gv/*.java $(DESTDIR)/$(libdir)/@PACKAGE@/
	cp $(top_srcdir)/tclpkg/gv/*.pm $(DESTDIR)/$(libdir)/@PACKAGE@/
	cp $(top_srcdir)/tclpkg/gv/*.py $(DESTDIR)/$(libdir)/@PACKAGE@/
	
if WITH_TCL
all-local: pkgIndex.tcl

pkgIndex.tcl: libgv_tcl.la
	sh $(top_srcdir)/tclpkg/mkpkgindex.sh libgv_tcl.la gv $(VERSION)
endif

test: test_java test_perl test_python test_ruby test_tcl

test_java: libgv_java.la
	(cd java; \
		ln -fs ../.libs/libgv_java.so libgv.so; \
		cp -f ../*.java .; \
		javac *.java; \
		LD_LIBRARY_PATH=. java test)

test_perl: libgv_perl.la
	(cd perl; \
		ln -fs ../.libs/libgv_perl.so gv.so; \
		cp -f ../*.pl ../*.pm .; \
		./test.pl)

test_python: libgv_python.la
	(cd python; \
		ln -fs ../.libs/libgv_python.so _gv.so; \
		cp -f ../*.py .; \
		PYTHONPATH=. ./test.py)

test_ruby: libgv_ruby.la
	(cd ruby; \
		ln -fs ../.libs/libgv_ruby.so gv.so; \
		cp -f ../*.rb .; \
		./test.rb)

test_tcl: libgv_tcl.la
	(cd tcl; \
		ln -fs ../.libs/libgv_tcl.so libgv_tcl.so; \
		cp -f ../*.tcl .; \
		./test.tcl)

EXTRA_DIST = gv.i \
	SWIGTYPE_p_Agraph_t.java SWIGTYPE_p_Agnode_t.java SWIGTYPE_p_Agedge_t.java \
	gv.java gvJNI.java gv.pm gv.py
	

CLEANFILES = java/*.dot java/*.png java/SWIG*.java java/gv*.java java/*.class java/*.so \
	perl/gv.pm perl/gv.so perl/*.dot perl/*.png \
	python/gv.py python/gv.pyc python/_gv.so python/*.dot python/*.png \
	ruby/gv.so ruby/*.dot ruby/*.png \
	tcl/libgv_tcl.so tcl/*.dot tcl/*.png pkgIndex.tcl

DISTCLEANFILES = gv_java.cpp gv_perl.cpp gv_python.cpp gv_ruby.cpp gv_tcl.cpp \
	SWIG*.java gv.java gvJNI.java gv.pm gv.py
