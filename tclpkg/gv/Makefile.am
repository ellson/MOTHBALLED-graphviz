## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/lib/gvc \
	-I$(top_srcdir)/lib/common \
	-I$(top_srcdir)/lib/graph \
	-I$(top_srcdir)/lib/cdt \
	-I$(top_srcdir)/lib/pathplan $(PERL_INCLUDES) $(PHP_INCLUDES) $(PYTHON_INCLUDES) $(RUBY_INCLUDES) $(TCL_INCLUDES) 

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
if WITH_SWIG
if WITH_CSHARP
pkglib_LTLIBRARIES += libgv_csharp.la
endif
if WITH_GUILE
pkglib_LTLIBRARIES += libgv_guile.la
endif
#if WITH_IO
#pkglib_LTLIBRARIES += libgv_io.la
#endif
if WITH_JAVA
pkglib_LTLIBRARIES += libgv_java.la
endif
if WITH_PERL
pkglib_LTLIBRARIES += libgv_perl.la
endif
if WITH_PHP
pkglib_LTLIBRARIES += libgv_php.la
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
endif

libgv_csharp_la_SOURCES = gv.cpp gv_csharp.cpp plugins.c 
libgv_csharp_la_LIBADD = $(BASELIBS) @CSHARP_LIBS@

libgv_guile_la_SOURCES = gv.cpp gv_guile.cpp plugins.c 
libgv_guile_la_LIBADD = $(BASELIBS) @GUILE_LIBS@

#libgv_io_la_SOURCES = gv.cpp gv_io.cpp plugins.c 
#libgv_io_la_LIBADD = $(BASELIBS) @IO_LIBS@

libgv_java_la_SOURCES = gv.cpp gv_java.cpp plugins.c 
libgv_java_la_LIBADD = $(BASELIBS) @JAVA_LIBS@

libgv_perl_la_SOURCES = gv.cpp gv_perl.cpp plugins.c 
libgv_perl_la_LIBADD = $(BASELIBS) @PERL_LIBS@

libgv_php_la_SOURCES = gv.cpp gv_php.cpp plugins.c 
libgv_php_la_LIBADD = $(BASELIBS) @PHP_LIBS@

libgv_python_la_SOURCES = gv.cpp gv_python.cpp plugins.c 
libgv_python_la_LIBADD = $(BASELIBS) @PYTHON_LIBS@

libgv_ruby_la_SOURCES = gv.cpp gv_ruby.cpp plugins.c 
libgv_ruby_la_LIBADD = $(BASELIBS) @RUBY_LIBS@

libgv_tcl_la_SOURCES = gv.cpp gv_tcl.cpp plugins.c 
libgv_tcl_la_LIBADD = $(BASELIBS) @TCL_LIBS@

gv_csharp.cpp: gv.i
	$(SWIG) -c++ -csharp -o gv_csharp.cpp gv.i

gv_guile.cpp: gv.i
	$(SWIG) -c++ -guile -o gv_guile.cpp gv.i

#gv_io.cpp: gv.i
#	$(SWIG) -c++ -io -o gv_io.cpp gv.i

gv_java.cpp: gv.i
	$(SWIG) -c++ -java -o gv_java.cpp gv.i

#gv_ocaml.cpp: gv.i
#	$(SWIG) -c++ -ocaml -o gv_ocaml.cpp gv.i

gv_perl.cpp: gv.i
	$(SWIG) -c++ -perl -o gv_perl.cpp gv.i

gv_php.cpp: gv.i
	$(SWIG) -c++ -php -o gv_php.cpp gv.i

gv_python.cpp: gv.i
	$(SWIG) -c++ -python -o gv_python.cpp gv.i

gv_ruby.cpp: gv.i
	$(SWIG) -c++ -ruby -o gv_ruby.cpp gv.i

gv_tcl.cpp: gv.i
	$(SWIG) -c++ -tcl -namespace -o gv_tcl.cpp gv.i

install-exec-hook:
	cp $(top_srcdir)/tclpkg/gv/*.cs $(DESTDIR)/$(libdir)/@PACKAGE@/
	cp $(top_srcdir)/tclpkg/gv/*.java $(DESTDIR)/$(libdir)/@PACKAGE@/
	cp $(top_srcdir)/tclpkg/gv/*.php $(DESTDIR)/$(libdir)/@PACKAGE@/
	cp $(top_srcdir)/tclpkg/gv/php_gv.h $(DESTDIR)/$(libdir)/@PACKAGE@/
	cp $(top_srcdir)/tclpkg/gv/*.pm $(DESTDIR)/$(libdir)/@PACKAGE@/
	cp $(top_srcdir)/tclpkg/gv/*.py $(DESTDIR)/$(libdir)/@PACKAGE@/
#	cp $(top_srcdir)/tclpkg/gv/*.ml $(DESTDIR)/$(libdir)/@PACKAGE@/
#	cp $(top_srcdir)/tclpkg/gv/*.mli $(DESTDIR)/$(libdir)/@PACKAGE@/
	
if WITH_TCL
all-local: pkgIndex.tcl

pkgIndex.tcl: libgv_tcl.la
	sh $(top_srcdir)/tclpkg/mkpkgindex.sh libgv_tcl.la gv $(VERSION)
endif

test: test_csharp test_guile test_java test_ocaml \
	test_perl test_php test_python test_ruby test_tcl

test_csharp: libgv_csharp.la
	(cd csharp; \
		ln -fs ../.libs/libgv_csharp.so libgv.so; \
		cp -f ../*.csharp .; \
		$(CSHARP) *.csharp; \
		LD_LIBRARY_PATH=. csharp test)

test_guile: libgv_guile.la
	(cd guile; \
		ln -fs ../.libs/libgv_guile.so libgv.so; \
		cp -f ../*.guile .; \
		guilec *.guile; \
		LD_LIBRARY_PATH=. guile test)

#test_io: libgv_io.la
#	(cd io; \
#		ln -fs ../.libs/libgv_io.so libgv.so; \
#		cp -f ../*.io .; \
#		ioc *.io; \
#		LD_LIBRARY_PATH=. io test)

test_java: libgv_java.la
	(cd java; \
		ln -fs ../.libs/libgv_java.so libgv.so; \
		cp -f ../*.java .; \
		javac *.java; \
		LD_LIBRARY_PATH=. java test)

#test_ocaml: libgv_ocaml.la
#	(cd ocaml; \
#		ln -fs ../.libs/libgv_ocaml.so libgv.so; \
#		cp -f ../*.ocaml .; \
#		ocamlc *.ocaml; \
#		LD_LIBRARY_PATH=. ocaml test)

test_perl: libgv_perl.la
	(cd perl; \
		ln -fs ../.libs/libgv_perl.so gv.so; \
		cp -f ../*.pl ../*.pm .; \
		./test.pl)

test_php: libgv_php.la
	(cd php; \
		ln -fs ../.libs/libgv_php.so libgv.so; \
		cp -f ../*.php .; \
		./test.php)

test_python: libgv_python.la
	(cd python; \
		ln -fs ../.libs/libgv_python.so _gv.so; \
		cp -f ../*.py .; \
		PYTHONPATH=. $PYTHON test.py)

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
	gv.cs gvPINVOKE.cs SWIGTYPE_p_Agedge_t.cs SWIGTYPE_p_Agnode_t.cs SWIGTYPE_p_Agraph_t.cs \
	gv.java gvJNI.java SWIGTYPE_p_Agedge_t.java SWIGTYPE_p_Agnode_t.java SWIGTYPE_p_Agraph_t.java \
	gv.pm \
	gv.php php_gv.h \
	gv.py \
	gv.ml gv.mli \
	demo/*
	

CLEANFILES = csharp/SWIG*.cs csharp/gv*.cs csharp/*.dot csharp/*.png csharp/*.so \
	java/*.dot java/*.png java/SWIG*.java java/gv*.java java/*.class java/*.so \
	perl/gv.pm perl/*.so perl/*.dot perl/*.png \
	php/gv.php php/php_gv.h php/*.so
	python/gv.py python/gv.pyc python/*.so python/*.dot python/*.png \
	ruby/*.so ruby/*.dot ruby/*.png \
	tcl/*.so tcl/*.dot tcl/*.png pkgIndex.tcl

DISTCLEANFILES = gv_csharp.cpp SWIG*.cs gv*.cs \
	gv_java.cpp SWIG*.java gv.java gvJNI.java \
	gv_perl.cpp gv.pm \
	gv_php.cpp gv.php php_gv.h \
	gv_python.cpp gv.py \
	gv.ml gv.mli \
	gv_ruby.cpp \
	gv_tcl.cpp
