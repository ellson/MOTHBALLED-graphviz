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

CSHARP_cpp = gv_csharp.cpp
CSHARP_hdr = gv.cs gvPINVOKE.cs SWIGTYPE_p_Agraph_t.cs SWIGTYPE_p_Agnode_t.cs SWIGTYPE_p_Agedge_t.cs
libgv_csharp_la_SOURCES = gv.cpp gv.i
libgv_csharp_la_DEPENDENCIES = gv.cpp $(CSHARP_cpp)
libgv_csharp_la_LIBADD = $(BASELIBS) @CSHARP_LIBS@
$(CSHARP_cpp) $(CSHARP_hdr): gv.i
	$(SWIG) -c++ -csharp -o $(CSHARP_cpp) gv.i

GUILE_cpp = gv_guile.cpp
GUILE_hdr = 
libgv_guile_la_SOURCES = gv.cpp gv.i
libgv_guile_la_DEPENDENCIES = gv.cpp $(GUILE_cp)
libgv_guile_la_LIBADD = $(BASELIBS) @GUILE_LIBS@
$(GUILE_cp) $(GUILE_hdr): gv.i
	$(SWIG) -c++ -guile -o $(GUILE_cp) gv.i

IO_cpp = gv_io.cpp
IO_hdr = 
libgv_io_la_SOURCES = gv.cpp gv.i
libgv_io_la_DEPENDENCIES = gv.cpp $(IO_cpp)
libgv_io_la_LIBADD = $(BASELIBS) @IO_LIBS@
$(IO_cpp) $(IO_hdr): gv.i
	$(SWIG) -c++ -io -o $(IO_cpp) gv.i

JAVA_cpp = gv_java.cpp
JAVA_hdr = gv.java gvJNI.java SWIGTYPE_p_Agraph_t.java SWIGTYPE_p_Agnode_t.java SWIGTYPE_p_Agedge_t.java
libgv_java_la_SOURCES = gv.cpp gv.i
libgv_java_la_DEPENDENCIES = gv.cpp $(JAVA_cpp)
libgv_java_la_LIBADD = $(BASELIBS) @JAVA_LIBS@
$(JAVA_cpp) $(JAVA_hdr): gv.i
	$(SWIG) -c++ -java -o $(JAVA_cpp) gv.i

OCAML_cpp = gv_ocaml.cpp
OCAML_hdr = gv.ml gv.mli
libgv_ocaml_la_SOURCES = gv.cpp gv.i
libgv_ocaml_la_DEPENDENCIES = gv.cpp $(OCAML_cpp)
libgv_ocaml_la_LIBADD = $(BASELIBS) @IO_LIBS@
$(OCAML_cpp) $(OCAML_hdr): gv.i
	$(SWIG) -c++ -ocaml -o $(OCAML_cpp) gv.i

PERL_cpp = gv_perl.cpp
PERL_hdr = gv.pm
libgv_perl_la_SOURCES = gv.cpp gv.i
libgv_perl_la_DEPENDENCIES = gv.cpp $(PERL_cpp)
libgv_perl_la_LIBADD = $(BASELIBS) @PERL_LIBS@
$(PERL_cpp) $(PERL_hdr): gv.i
	$(SWIG) -c++ -perl -o $(PERL_cpp) gv.i

PHP_cpp = gv_php.cpp
PHP_hdr = gv.php php_gv.h
libgv_php_la_SOURCES = gv.cpp gv.i
libgv_php_la_DEPENDENCIES = gv.cpp $(PHP_cpp)
libgv_php_la_LIBADD = $(BASELIBS) @PHP_LIBS@
$(PHP_cpp) $(PHP_hdr): gv.i
	$(SWIG) -c++ -php -o $(PHP_cpp) gv.i

PYTHON_cpp = gv_python.cpp
PYTHON_hdr = gv.py
libgv_python_la_SOURCES = gv.cpp gv.i
libgv_python_la_DEPENDENCIES = gv.cpp $(PYTHON_cpp)
libgv_python_la_LIBADD = $(BASELIBS) @PYTHON_LIBS@
$(PYTHON_cpp) $(PYTHON_hdr): gv.i
	$(SWIG) -c++ -python -o $(PYTHON_cpp) gv.i

RUBY_cpp = gv_ruby.cpp
RUBY_hdr = 
libgv_ruby_la_SOURCES = gv.cpp gv.i
libgv_ruby_la_DEPENDENCIES = gv.cpp $(RUBY_cpp)
libgv_ruby_la_LIBADD = $(BASELIBS) @RUBY_LIBS@
$(RUBY_cpp) $(RUBY_hdr): gv.i
	$(SWIG) -c++ -ruby -o $(RUBY_cpp) gv.i

TCL_cpp = gv_tcl.cpp
TCL_hdr = 
libgv_tcl_la_SOURCES = gv.cpp gv.i
libgv_tcl_la_DEPENDENCIES = gv.cpp $(TCL_cpp)
libgv_tcl_la_LIBADD = $(top_builddir)/tclpkg/tclstubs/libtclstubs.la $(BASELIBS) @TCL_LIBS@
$(TCL_cpp) $(TCL_hdr): gv.i
	$(SWIG) -c++ -tcl -namespace -o $(TCL_cpp) gv.i


pkginclude_DATA = gv.i gv.cpp
pkglib_LTLIBRARIES = 
TESTS =
if WITH_SWIG
if WITH_CSHARP
pkginclude_DATA += $(CSHARP_hdr)
pkglib_LTLIBRARIES += libgv_csharp.la
TESTS += test_csharp
endif
if WITH_GUILE
pkginclude_DATA += $(GUILE_hdr)
pkglib_LTLIBRARIES += libgv_guile.la
TESTS += test_guile
endif
if WITH_IO
pkginclude_DATA += $(IO_hdr)
pkglib_LTLIBRARIES += libgv_io.la
TESTS += test_io
endif
if WITH_JAVA
pkginclude_DATA += $(JAVA_hdr)
pkglib_LTLIBRARIES += libgv_java.la
TESTS += test_java
endif
if WITH_OCAML
pkginclude_DATA += $(OCAML_hdr)
pkglib_LTLIBRARIES += libgv_ocaml.la
TESTS += test_ocaml
endif
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
		ln -fs ../.libs/libgv_csharp.so libgv.so; \
		ln -fs ../*.cs .; \
		$(CSHARP) *.cs; \
		LD_LIBRARY_PATH=. $(CSHARP) test)

.PHONY: test_guile
test_guile: libgv_guile.la
	-(mkdir -p test_guile; cd test_guile; \
		ln -fs ../.libs/libgv_guile.so libgv.so; \
		ln -fs ../*.guile .; \
		$(GUILE) -s test.guile)

.PHONY: test_io
test_io: libgv_io.la
	-(mkdir -p test_io; cd test_io; \
		ln -fs ../.libs/libgv_io.so libgv.so; \
		cp -f ../*.io .; \
		ioc *.io; \
		LD_LIBRARY_PATH=. io test)

.PHONY: test_java
test_java: libgv_java.la
	-(mkdir -p test_java; cd test_java; \
		ln -fs ../.libs/libgv_java.so libgv.so; \
		ln -fs ../*.java .; \
		javac *.java; \
		LD_LIBRARY_PATH=. CLASSPATH=. java test)

.PHONY: test_ocaml
test_ocaml: libgv_ocaml.la
	-(mkdir -p test_ocaml; cd test_ocaml; \
		ln -fs ../.libs/libgv_ocaml.so libgv.so; \
		ln -fs ../*.ocaml .; \
		ocamlc *.ocaml; \
		LD_LIBRARY_PATH=. ocaml test)

.PHONY: test_perl
test_perl: libgv_perl.la
	-(mkdir -p test_perl; cd test_perl; \
		ln -fs ../.libs/libgv_perl.so gv.so; \
		ln -fs ../*.p[lm] .; \
		$(PERL) ./test.pl)

.PHONY: test_php
test_php: libgv_php.la
	-(mkdir -p test_php; cd test_php; \
		ln -fs ../.libs/libgv_php.so libgv.so; \
		ln -fs ../*.php .; \
		$(PHP) ./test.php)

.PHONY: test_python
test_python: libgv_python.la
	-(mkdir -p test_python; cd test_python; \
		ln -fs ../.libs/libgv_python.so _gv.so; \
		ln -fs ../*.py .; \
		PYTHONPATH=. $(PYTHON) test.py)

.PHONY: test_ruby
test_ruby: libgv_ruby.la
	-(mkdir -p test_ruby; cd test_ruby; \
		ln -fs ../.libs/libgv_ruby.so gv.so; \
		ln -fs ../*.rb .; \
		$(RUBY) test.rb)

.PHONY: test_tcl
test_tcl: libgv_tcl.la
	-(mkdir -p test_tcl; cd test_tcl; \
		ln -fs ../.libs/libgv_tcl.so libgv_tcl.so; \
		ln -fs ../*.tcl .; \
		$(TCLSH) ./test.tcl)

EXTRA_DIST = demo/* 

CLEANFILES = test_*/*

DISTCLEANFILES = \
	$(CSHARP_cpp) $(CSHARP_hdr) \
	$(GUILE_cp) $(GUILE_hdr) \
	$(IO_cpp) $(IO_hdr) \
	$(JAVA_cpp) $(JAVA_hdr) \
	$(OCAML_cpp) $(OCAML_hdr) \
	$(PERL_cpp) $(PERL_hdr) \
	$(PHP_cpp) $(PHP_hdr) \
	$(PYTHON_cpp) $(PYTHON_hdr) \
	$(RUBY_cpp) $(RUBY_hdr) \
	$(TCL_cpp) $(TCL_hdr)
