## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/lib/gvc \
	-I$(top_srcdir)/lib/common \
	-I$(top_srcdir)/lib/graph \
	-I$(top_srcdir)/lib/cdt \
	-I$(top_srcdir)/lib/pathplan $(PERL_INCLUDES) $(PHP_INCLUDES) $(PYTHON_INCLUDES) $(RUBY_INCLUDES) $(TCL_INCLUDES)

LIBS =  @LIBS@ -lc

BASELIBS =  $(top_builddir)/lib/gvc/libgvc.la

#CSHARP_hdr = gv.cs gvPINVOKE.cs SWIGTYPE_p_Agraph_t.cs SWIGTYPE_p_Agnode_t.cs SWIGTYPE_p_Agedge_t.cs
#libgv_csharp_la_SOURCES = gv.cpp gv_csharp.cpp
#libgv_csharp_la_LIBADD = $(BASELIBS) @CSHARP_LIBS@ 
#gv_csharp.cpp $(CSHARP_hdr): gv.i
#	$(SWIG) -c++ -csharp -namespace gv -o gv_csharp.cpp gv.i

#GUILE_hdr = 
#libgv_guile_la_SOURCES = gv.cpp gv.gv_guile.cpp
#libgv_guile_la_LIBADD = $(BASELIBS) @GUILE_LIBS@
#gv_guile.cpp $(GUILE_hdr): gv.i
#	$(SWIG) -c++ -guile -o gv_guile.cpp gv.i

#IO_hdr = 
#libgv_io_la_SOURCES = gv.cpp gv_io.cpp
#libgv_io_la_LIBADD = $(BASELIBS) @IO_LIBS@
#gv_io.cpp $(IO_hdr): gv.i
#	$(SWIG) -c++ -io -o gv_io.cpp gv.i

JAVA_hdr = gv.java gvJNI.java SWIGTYPE_p_Agraph_t.java SWIGTYPE_p_Agnode_t.java SWIGTYPE_p_Agedge_t.java
libgv_java_la_SOURCES = gv.cpp gv_java.cpp
libgv_java_la_LIBADD = $(BASELIBS) @JAVA_LIBS@
gv_java.cpp $(JAVA_hdr): gv.i
	$(SWIG) -c++ -java -o gv_java.cpp gv.i

#OCAML_hdr = gv.ml gv.mli
#libgv_ocaml_la_SOURCES = gv.cpp gv_ocaml.cpp
#libgv_ocaml_la_LIBADD = $(BASELIBS) @IO_LIBS@
#gv_ocaml.cpp $(OCAML_hdr): gv.i
#	$(SWIG) -c++ -ocaml -o gv_ocaml.cpp gv.i

PERL_hdr = gv.pm
libgv_perl_la_SOURCES = gv.cpp gv_perl.cpp
libgv_perl_la_LIBADD = $(BASELIBS) @PERL_LIBS@
gv_perl.cpp $(PERL_hdr): gv.i
	$(SWIG) -c++ -perl -o gv_perl.cpp gv.i

PHP_hdr = gv.php php_gv.h
libgv_php_la_SOURCES = gv.cpp gv_php.cpp
libgv_php_la_LIBADD = $(BASELIBS) @PHP_LIBS@
gv_php.cpp $(PHP_hdr): gv.i
	$(SWIG) -c++ -php -o gv_php.cpp gv.i

PYTHON_hdr = gv.py
libgv_python_la_SOURCES = gv.cpp gv_python.cpp
libgv_python_la_LIBADD = $(BASELIBS) @PYTHON_LIBS@
gv_python.cpp $(PYTHON_hdr): gv.i
	$(SWIG) -c++ -python -o gv_python.cpp gv.i

RUBY_hdr = 
libgv_ruby_la_SOURCES = gv.cpp gv_ruby.cpp
libgv_ruby_la_LIBADD = $(BASELIBS) @RUBY_LIBS@
gv_ruby.cpp $(RUBY_hdr): gv.i
	$(SWIG) -c++ -ruby -o gv_ruby.cpp gv.i

TCL_hdr = 
libgv_tcl_la_SOURCES = gv.cpp gv_tcl.cpp
libgv_tcl_la_LIBADD = $(top_builddir)/tclpkg/tclstubs/libtclstubs.la $(BASELIBS) @TCL_LIBS@
gv_tcl.cpp $(TCL_hdr): gv.i
	$(SWIG) -c++ -tcl -namespace -o gv_tcl.cpp gv.i

pkginclude_DATA = gv.i gv.cpp
pkglib_LTLIBRARIES = 
TESTS =
if WITH_SWIG
#if WITH_CSHARP
#pkginclude_DATA += $(CSHARP_hdr)
#pkglib_LTLIBRARIES += libgv_csharp.la
#TESTS += test_csharp
#endif
#if WITH_GUILE
#pkginclude_DATA += $(GUILE_hdr)
#pkglib_LTLIBRARIES += libgv_guile.la
#TESTS += test_guile
#endif
#if WITH_IO
#pkginclude_DATA += $(IO_hdr)
#pkglib_LTLIBRARIES += libgv_io.la
#TESTS += test_io
#endif
if WITH_JAVA
pkginclude_DATA += $(JAVA_hdr)
pkglib_LTLIBRARIES += libgv_java.la
TESTS += test_java
endif
#if WITH_OCAML
#pkginclude_DATA += $(OCAML_hdr)
#pkglib_LTLIBRARIES += libgv_ocaml.la
#TESTS += test_ocaml
#endif
if WITH_PERL
pkginclude_DATA += $(PERL_hdr)
pkglib_LTLIBRARIES += libgv_perl.la
TESTS += test_perl
endif
if WITH_PHP
pkginclude_DATA += $(PHP_hdr)
pkglib_LTLIBRARIES += libgv_php.la
TESTS += test_php
endif
if WITH_PYTHON
pkginclude_DATA += $(PYTHON_hdr)
pkglib_LTLIBRARIES += libgv_python.la
TESTS += test_python
endif
if WITH_RUBY
pkginclude_DATA += $(RUBY_hdr)
pkglib_LTLIBRARIES += libgv_ruby.la
TESTS += test_ruby
endif
if WITH_TCL
pkginclude_DATA += $(TCL_hdr)
pkglib_LTLIBRARIES += libgv_tcl.la
TESTS += test_tcl
endif
endif

if WITH_TCL
all-local: pkgIndex.tcl

pkgIndex.tcl: libgv_tcl.la
	sh $(top_srcdir)/tclpkg/mkpkgindex.sh libgv_tcl.la gv $(VERSION)
endif

test: $(TESTS)

.PHONY: test_csharp
test_csharp: libgv_csharp.la
	-(mkdir -p test_csharp; cd test_csharp; \
		ln -fs ../.libs/libgv_csharp.so libgv_csharp.so; \
		ln -fs ../*.dot ../*.cs .; \
		$(CSHARP) *.cs -out:test.exe; \
		mono test.exe)

.PHONY: test_guile
test_guile: libgv_guile.la
	-(mkdir -p test_guile; cd test_guile; \
		ln -fs ../.libs/libgv_guile.so libgv.so; \
		ln -fs ../*.dot ../*.guile .; \
		$(GUILE) -s test.guile)

.PHONY: test_io
test_io: libgv_io.la
	-(mkdir -p test_io; cd test_io; \
		ln -fs ../.libs/libgv_io.so libgv.so; \
		ln -fs ../*.dot ../*.io .; \
		ioc *.io; \
		LD_LIBRARY_PATH=. io test)

.PHONY: test_java
test_java: libgv_java.la
	-(mkdir -p test_java; cd test_java; \
		ln -fs ../.libs/libgv_java.so libgv.so; \
		ln -fs ../*.dot ../*.java .; \
		javac *.java; \
		LD_LIBRARY_PATH=. CLASSPATH=. java test)

.PHONY: test_ocaml
test_ocaml: libgv_ocaml.la
	-(mkdir -p test_ocaml; cd test_ocaml; \
		ln -fs ../.libs/libgv_ocaml.so libgv.so; \
		ln -fs ../*.dot ../*.ocaml .; \
		ocamlc *.ocaml; \
		LD_LIBRARY_PATH=. ocaml test)

.PHONY: test_perl
test_perl: libgv_perl.la
	-(mkdir -p test_perl; cd test_perl; \
		ln -fs ../.libs/libgv_perl.so gv.so; \
		ln -fs ../*.dot ../*.p[lm] .; \
		$(PERL) ./test.pl)

.PHONY: test_php
test_php: libgv_php.la
	-(mkdir -p test_php; cd test_php; \
		ln -fs ../.libs/libgv_php.so libgv.so; \
		ln -fs ../*.dot ../*.php .; \
		$(PHP) ./test.php)

.PHONY: test_python
test_python: libgv_python.la
	-(mkdir -p test_python; cd test_python; \
		ln -fs ../.libs/libgv_python.so _gv.so; \
		ln -fs ../*.dot ../*.py .; \
		PYTHONPATH=. $(PYTHON) test.py)

.PHONY: test_ruby
test_ruby: libgv_ruby.la
	-(mkdir -p test_ruby; cd test_ruby; \
		ln -fs ../.libs/libgv_ruby.so gv.so; \
		ln -fs ../*.dot ../*.rb .; \
		$(RUBY) test.rb)

.PHONY: test_tcl
test_tcl: libgv_tcl.la
	-(mkdir -p test_tcl; cd test_tcl; \
		ln -fs ../.libs/libgv_tcl.so libgv_tcl.so; \
		ln -fs ../*.dot ../*.tcl .; \
		$(TCLSH) ./test.tcl)

EXTRA_DIST = demo/* gv.cpp gv.i \
	gv_java.cpp $(JAVA_hdr) \
	gv_perl.cpp $(PERL_hdr) \
	gv_php.cpp $(PHP_hdr) \
	gv_python.cpp $(PYTHON_hdr) \
	gv_ruby.cpp $(RUBY_hdr) \
	gv_tcl.cpp $(TCL_hdr)

#	gv_csharp.cpp $(CSHARP_hdr) \
#	gv_guile.cpp $(GUILE_hdr) \
#	gv_io.cpp $(IO_hdr) \
#	gv_ocaml.cpp $(OCAML_hdr) \
#
#
CLEANFILES = test_*/*

DISTCLEANFILES = \
	gv_csharp.cpp $(CSHARP_hdr) \
	gv_guile.cpp $(GUILE_hdr) \
	gv_io.cpp $(IO_hdr) \
	gv_java.cpp $(JAVA_hdr) \
	gv_ocaml.cpp $(OCAML_hdr) \
	gv_perl.cpp $(PERL_hdr) \
	gv_php.cpp $(PHP_hdr) \
	gv_python.cpp $(PYTHON_hdr) \
	gv_ruby.cpp $(RUBY_hdr) \
	gv_tcl.cpp $(TCL_hdr)
