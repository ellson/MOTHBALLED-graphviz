# $Id$ $Revision$
## Process this file with automake to produce Makefile.in

SUBDIRS = demo

pdfdir = $(pkgdatadir)/doc/pdf

AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/lib/gvc \
	-I$(top_srcdir)/lib/common \
	-I$(top_srcdir)/lib/graph \
	-I$(top_srcdir)/lib/cdt \
	-I$(top_srcdir)/lib/pathplan $(GUILE_INCLUDES) $(LUA_INCLUDES) $(OCAML_INCLUDES) $(PERL_INCLUDES) $(PHP_INCLUDES) $(PYTHON_INCLUDES) $(R_CFLAGS) $(RUBY_INCLUDES) $(TCL_INCLUDES)

LIBS =  @LIBS@ -lc

BASESOURCES = gv.cpp gv.i
BASELIBS = $(top_builddir)/lib/gvc/libgvc.la

pkgsharpdir = $(pkglibdir)/sharp
SHARP_data = gv.cs gvPINVOKE.cs SWIGTYPE_p_Agraph_t.cs SWIGTYPE_p_Agnode_t.cs SWIGTYPE_p_Agedge_t.cs SWIGTYPE_p_Agsym_t.cs SWIGTYPE_p_FILE.cs
nodist_libgv_sharp_la_SOURCES = gv_sharp.cpp $(SHARP_data)
libgv_sharp_la_SOURCES = $(BASESOURCES) gv_dummy_init.cpp
libgv_sharp_la_LIBADD = $(BASELIBS) @SHARP_LIBS@ 
libgv_sharp_la_LDFLAGS = -module -avoid-version
$(SHARP_data): gv_sharp.cpp
gv_sharp.cpp: gv.i
	$(SWIG) -c++ -csharp -namespace gv -o gv_sharp.cpp $(srcdir)/gv.i

pkgguiledir = $(pkglibdir)/guile
GUILE_data = 
nodist_libgv_guile_la_SOURCES = gv_guile.cpp $(GUILE_data)
libgv_guile_la_SOURCES = $(BASESOURCES) gv_dummy_init.cpp
libgv_guile_la_LIBADD = $(BASELIBS) @GUILE_LIBS@
libgv_guile_la_LDFLAGS = -module -avoid-version
$(GUILE_data): gv_guile.cpp
gv_guile.cpp: gv.i
	$(SWIG) -c++ -guile -o gv_guile.cpp $(srcdir)/gv.i
	$(SED) -e 's/SWIG_GUILE_INIT_STATIC void/void/' <gv_guile.cpp >t
	mv -f t gv_guile.cpp

pkgiodir = $(pkglibdir)/io
IO_data = 
nodist_libgv_io_la_SOURCES = gv_io.cpp $(IO_data)
libgv_io_la_SOURCES = $(BASESOURCES) gv_dummy_init.cpp
libgv_io_la_LIBADD = $(BASELIBS) @IO_LIBS@
libgv_io_la_LDFLAGS = -module -avoid-version
$(IO_data): gv_io.cpp
gv_io.cpp: gv.i
	$(SWIG) -c++ -io -o gv_io.cpp $(srcdir)/gv.i

pkgjavadir = $(pkglibdir)/java
JAVA_data = gv.java gvJNI.java SWIGTYPE_p_Agraph_t.java SWIGTYPE_p_Agnode_t.java SWIGTYPE_p_Agedge_t.java SWIGTYPE_p_Agsym_t.java SWIGTYPE_p_FILE.java
nodist_libgv_java_la_SOURCES = gv_java.cpp
libgv_java_la_SOURCES = $(BASESOURCES)  gv_dummy_init.cpp
libgv_java_la_LIBADD = $(BASELIBS) @JAVA_LIBS@
libgv_java_la_LDFLAGS = -module -avoid-version
$(JAVA_data): gv_java.cpp
gv_java.cpp: gv.i
	$(SWIG) -c++ -java -o gv_java.cpp $(srcdir)/gv.i

pkgluadir = $(pkglibdir)/lua
LUA_data = 
nodist_libgv_lua_la_SOURCES = gv_lua.cpp $(LUA_data)
libgv_lua_la_SOURCES = $(BASESOURCES) gv_dummy_init.cpp
libgv_lua_la_LIBADD = $(BASELIBS) @LUA_LIBS@
libgv_lua_la_LDFLAGS = -module -avoid-version
$(LUA_data): gv_lua.cpp
gv_lua.cpp: gv.i
	$(SWIG) -c++ -lua -o gv_lua.cpp $(srcdir)/gv.i

pkgocamldir = $(pkglibdir)/ocaml
OCAML_data = gv.cmo gv.cma gv.cmi gv.mli META.gv
if WITH_OCAMLOPT
OCAML_data += gv.a gv.cmx gv.cmxa
endif
nodist_libgv_ocaml_la_SOURCES = gv_ocaml.cpp $(OCAML_data)
libgv_ocaml_la_SOURCES = $(BASESOURCES) gv_dummy_init.cpp
libgv_ocaml_la_LIBADD = $(BASELIBS) @OCAML_LIBS@
libgv_ocaml_la_LDFLAGS = -module -avoid-version
gv.ml gv.mli: gv_ocaml.cpp
gv_ocaml.cpp: gv.i
	$(SWIG) -c++ -ocaml -o gv_ocaml.cpp $(srcdir)/gv.i
swig.mli:
	$(SWIG) -ocaml -co swig.mli
swig.ml:
	$(SWIG) -ocaml -co swig.ml
swig.cmi: swig.mli
	ocamlc -c swig.mli
swig.cmo: swig.ml swig.cmi
	ocamlc -c swig.ml
gv.cmi: gv.mli swig.cmi
	ocamlc -c gv.mli
gv.cmo: gv.ml gv.cmi
	ocamlc -c gv.ml
gv.cma: swig.cmo gv.cmo
	ocamlc -a -dllib dllgv.so -custom -o gv.cma swig.cmo gv.cmo
gv.cmx: gv.ml gv.cma
	ocamlopt -c gv.ml
gv.cmxa gv.a: gv.cma
	ocamlopt -a -o gv.cmxa

pkgperldir = $(pkglibdir)/perl
PERL_data = gv.pm
nodist_libgv_perl_la_SOURCES = gv_perl.cpp $(PERL_data)
libgv_perl_la_SOURCES = $(BASESOURCES) gv_dummy_init.cpp
libgv_perl_la_LIBADD = $(BASELIBS) @PERL_LIBS@
libgv_perl_la_LDFLAGS = -module -avoid-version
$(PERL_data): gv_perl.cpp
gv_perl.cpp: gv.i
	$(SWIG) -c++ -perl -o gv_perl.cpp $(srcdir)/gv.i

pkgphpdir = $(pkglibdir)/php
PHP_data = gv.php
nodist_libgv_php_la_SOURCES = gv_php.cpp php_gv.h
libgv_php_la_SOURCES = $(BASESOURCES) gv_php_init.cpp
libgv_php_la_LIBADD = $(BASELIBS) @PHP_LIBS@
libgv_php_la_LDFLAGS = -module -avoid-version
gv.php: gv_php.cpp
php_gv.h: gv_php.cpp
gv_php.cpp: gv.i
	$(SWIG) -c++ -php5 -o gv_php.cpp $(srcdir)/gv.i

pkgpythondir = $(pkglibdir)/python
PYTHON_data = gv.py
nodist_libgv_python_la_SOURCES = gv_python.cpp $(PYTHON_data)
libgv_python_la_SOURCES = $(BASESOURCES) gv_dummy_init.cpp
libgv_python_la_LIBADD = $(BASELIBS) @PYTHON_LIBS@
libgv_python_la_LDFLAGS = -module -avoid-version
$(PYTHON_data): gv_python.cpp
gv_python.cpp: gv.i
	$(SWIG) -c++ -python -o gv_python.cpp $(srcdir)/gv.i

pkgpython23dir = $(pkglibdir)/python23
nodist_libgv_python23_la_SOURCES = $(nodist_libgv_python_la_SOURCES)
libgv_python23_la_SOURCES = $(BASESOURCES) gv_dummy_init.cpp
libgv_python23_la_LIBADD = $(BASELIBS) @PYTHON23_LIBS@
libgv_python23_la_LDFLAGS = -module -avoid-version

pkgpython24dir = $(pkglibdir)/python24
nodist_libgv_python24_la_SOURCES = $(nodist_libgv_python_la_SOURCES)
libgv_python24_la_SOURCES = $(BASESOURCES) gv_dummy_init.cpp
libgv_python24_la_LIBADD = $(BASELIBS) @PYTHON24_LIBS@
libgv_python24_la_LDFLAGS = -module -avoid-version

pkgpython25dir = $(pkglibdir)/python25
nodist_libgv_python25_la_SOURCES = $(nodist_libgv_python_la_SOURCES)
libgv_python25_la_SOURCES = $(BASESOURCES) gv_dummy_init.cpp
libgv_python25_la_LIBADD = $(BASELIBS) @PYTHON25_LIBS@
libgv_python25_la_LDFLAGS = -module -avoid-version

pkgRdir = $(pkglibdir)/R
R_data = 
nodist_libgv_R_la_SOURCES = gv_R.cpp $(R_data)
libgv_R_la_SOURCES = $(BASESOURCES) gv_dummy_init.cpp
libgv_R_la_LIBADD = $(BASELIBS) @R_LIBS@
libgv_R_la_LDFLAGS = -module -avoid-version
$(R_data): gv_R.cpp
gv_R.cpp: gv.i
	$(SWIG) -c++ -r -o gv_R.cpp $(srcdir)/gv.i

pkgrubydir = $(pkglibdir)/ruby
RUBY_data = 
nodist_libgv_ruby_la_SOURCES = gv_ruby.cpp $(RUBY_data)
libgv_ruby_la_SOURCES = $(BASESOURCES) gv_dummy_init.cpp
libgv_ruby_la_LIBADD = $(BASELIBS) @RUBY_LIBS@
libgv_ruby_la_LDFLAGS = -module -avoid-version
$(RUBY_data): gv_ruby.cpp
gv_ruby.cpp: gv.i
	$(SWIG) -c++ -ruby -o gv_ruby.cpp $(srcdir)/gv.i

pkgtcldir = $(pkglibdir)/tcl
TCL_data = 
nodist_libgv_tcl_la_SOURCES = gv_tcl.cpp $(TCL_data)
libgv_tcl_la_SOURCES = $(BASESOURCES) gv_dummy_init.cpp
libgv_tcl_la_LIBADD = $(top_builddir)/tclpkg/tclstubs/libtclstubs_C.la $(BASELIBS) @TCL_LIBS@
libgv_tcl_la_LDFLAGS = -module -avoid-version
$(TCL_data): gv_tcl.cpp
gv_tcl.cpp: gv.i
	$(SWIG) -c++ -tcl -namespace -o gv_tcl.cpp $(srcdir)/gv.i

pkginclude_DATA = gv.i gv.cpp
pkglib_LTLIBRARIES = 
man_MANS =
#pdf_DATA = 
DEVTSTS =
if WITH_SWIG
if WITH_SHARP
pkgsharp_DATA = $(SHARP_data)
man_MANS += gv_sharp.n
#pdf_DATA += gv_sharp.pdf
pkgsharp_LTLIBRARIES = libgv_sharp.la
DEVTSTS += test_sharp
endif
if WITH_GUILE
pkgguile_DATA = $(GUILE_data)
man_MANS += gv_guile.n
#pdf_DATA += gv_guile.pdf
pkgguile_LTLIBRARIES = libgv_guile.la
DEVTSTS += test_guile
endif
if WITH_IO
pkgio_DATA = $(IO_data)
man_MANS += gv_io.n
#pdf_DATA += gv_io.pdf
pkgio_LTLIBRARIES = libgv_io.la
DEVTSTS += test_io
endif
if WITH_JAVA
pkgjava_DATA = $(JAVA_data)
man_MANS += gv_java.n
#pdf_DATA += gv_java.pdf
pkgjava_LTLIBRARIES = libgv_java.la
DEVTSTS += test_java
endif
if WITH_LUA
pkglua_DATA = $(LUA_data)
man_MANS += gv_lua.n
#pdf_DATA += gv_lua.pdf
pkglua_LTLIBRARIES = libgv_lua.la
DEVTSTS += test_lua
endif
if WITH_OCAML
pkgocaml_DATA = $(OCAML_data)
man_MANS += gv_ocaml.n
#pdf_DATA += gv_ocaml.pdf
pkgocaml_LTLIBRARIES = libgv_ocaml.la
DEVTSTS += test_ocaml
endif
if WITH_PERL
pkgperl_DATA = $(PERL_data)
man_MANS += gv_perl.n
#pdf_DATA += gv_perl.pdf
pkgperl_LTLIBRARIES = libgv_perl.la
DEVTSTS += test_perl
endif
if WITH_PHP
pkgphp_DATA = $(PHP_data)
man_MANS += gv_php.n
#pdf_DATA += gv_php.pdf
pkgphp_LTLIBRARIES = libgv_php.la
DEVTSTS += test_php
endif
if WITH_PYTHON
pkgpython_DATA = $(PYTHON_data)
man_MANS += gv_python.n
#pdf_DATA += gv_python.pdf
pkgpython_LTLIBRARIES = libgv_python.la
DEVTSTS += test_python
endif
if WITH_PYTHON23
pkgpython23_DATA = $(PYTHON23_data)
man_MANS += gv_python.n
#pdf_DATA += gv_python.pdf
pkgpython23_LTLIBRARIES = libgv_python23.la
#DEVTSTS += test_python23
endif
if WITH_PYTHON24
pkgpython24_DATA = $(PYTHON24_data)
man_MANS += gv_python.n
#pdf_DATA += gv_python.pdf
pkgpython24_LTLIBRARIES = libgv_python24.la
#DEVTSTS += test_python24
endif
if WITH_PYTHON25
pkgpython25_DATA = $(PYTHON25_data)
man_MANS += gv_python.n
#pdf_DATA += gv_python.pdf
pkgpython25_LTLIBRARIES = libgv_python25.la
#DEVTSTS += test_python25
endif
if WITH_R
pkgR_DATA = $(R_data)
man_MANS += gv_R.n
#pdf_DATA += gv_R.pdf
pkgR_LTLIBRARIES = libgv_R.la
DEVTSTS += test_R
endif
if WITH_RUBY
pkgruby_DATA = $(RUBY_data)
man_MANS += gv_ruby.n
#pdf_DATA += gv_ruby.pdf
pkgruby_LTLIBRARIES = libgv_ruby.la
DEVTSTS += test_ruby
endif
if WITH_TCL
pkgtcl_DATA = $(TCL_data)
man_MANS += gv_tcl.n
#pdf_DATA += gv_tcl.pdf
pkgtcl_LTLIBRARIES = libgv_tcl.la
DEVTSTS += test_tcl

all-local: pkgIndex.tcl

pkgIndex.tcl: libgv_tcl.la
	sh $(top_srcdir)/tclpkg/mkpkgindex.sh libgv_tcl.la gv 0
endif
endif

man = gv_sharp.n gv_guile.n gv_io.n gv_java.n gv_lua.n gv_ocaml.n \
	gv_perl.n gv_php.n gv_python.n gv_R.n gv_ruby.n gv_tcl.n

#pdf = gv_sharp.pdf gv_guile.pdf gv_io.pdf gv_java.pdf gv_lua.pdf gv_ocaml.pdf \
#	gv_perl.pdf gv_php.pdf gv_python.pdf gv_R.pdf gv_ruby.pdf gv_tcl.pdf

$(man): gv.i gv_doc_writer.tcl gv_doc_template.tcl gv_doc_langs.tcl
	$(TCLSH) $(srcdir)/gv_doc_writer.tcl "$(srcdir)"

.n.pdf:
	-groff -Tps -man $< | ps2pdf - - >$@

install-data-hook:
if WITH_LUA
	(cd $(DESTDIR)$(pkgluadir);    rm -f gv.so;  $(LN_S) libgv_lua.so gv.so;)
endif
if WITH_PERL
	(cd $(DESTDIR)$(pkgperldir);   rm -f gv.so;  $(LN_S) libgv_perl.so gv.so;)
endif
if WITH_PHP
	(cd $(DESTDIR)$(pkgphpdir);   rm -f gv.so;  $(LN_S) libgv_php.so gv.so;)
endif
if WITH_PYTHON
	(cd $(DESTDIR)$(pkgpythondir); rm -f _gv.so; $(LN_S) libgv_python.so _gv.so;)
endif
if WITH_PYTHON23
	(cd $(DESTDIR)$(pkgpython23dir); rm -f _gv.so; $(LN_S) libgv_python23.so _gv.so;)
endif
if WITH_PYTHON24
	(cd $(DESTDIR)$(pkgpython24dir); rm -f _gv.so; $(LN_S) libgv_python24.so _gv.so;)
endif
if WITH_PYTHON25
	(cd $(DESTDIR)$(pkgpython25dir); rm -f _gv.so; $(LN_S) libgv_python25.so _gv.so;)
endif
if WITH_R
	(cd $(DESTDIR)$(pkgRdir);   rm -f gv.so;  $(LN_S) libgv_R.so gv.so;)
endif
if WITH_RUBY
	(cd $(DESTDIR)$(pkgrubydir);   rm -f gv.so;  $(LN_S) libgv_ruby.so gv.so;)
endif

uninstall-hook:
	rm -rf $(DESTDIR)$(pkgsharpdir);
	rm -rf $(DESTDIR)$(pkgguiledir);
	rm -rf $(DESTDIR)$(pkgiodir);
	rm -rf $(DESTDIR)$(pkgjavadir);
	rm -rf $(DESTDIR)$(pkgluadir);
	rm -rf $(DESTDIR)$(pkgperldir);
	rm -rf $(DESTDIR)$(pkgpythondir);
	rm -rf $(DESTDIR)$(pkgpython23dir);
	rm -rf $(DESTDIR)$(pkgpython24dir);
	rm -rf $(DESTDIR)$(pkgpython25dir);
	rm -rf $(DESTDIR)$(pkgRdir);
	rm -rf $(DESTDIR)$(pkgrubydir);
	rm -rf $(DESTDIR)$(pkgtcldir);
	rm -rf $(DESTDIR)$(pkgphpdir);

devtsts: $(DEVTSTS)

.PHONY: test_sharp
test_sharp: libgv_sharp.la
	-(mkdir -p test_sharp; cd test_sharp; \
		ln -fs ../.libs/libgv_sharp.so libgv_sharp.so; \
		ln -fs ../$(srcdir)/*.dot ../$(srcdir)/*.cs .; \
		$(SHARP) *.cs -out:test.exe; \
		mono test.exe)

.PHONY: test_guile
test_guile: libgv_guile.la
	-(mkdir -p test_guile; cd test_guile; \
		ln -fs ../.libs/libgv_guile.so libgv.so; \
		ln -fs ../$(srcdir)/*.dot ../$(srcdir)/*.guile .; \
		$(GUILE) -s test.guile)

.PHONY: test_io
test_io: libgv_io.la
	-(mkdir -p test_io; cd test_io; \
		ln -fs ../.libs/libgv_io.so libgv.so; \
		ln -fs ../$(srcdir)/*.dot ../$(srcdir)/*.io .; \
		ioc *.io; \
		LD_LIBRARY_PATH=. io test)

.PHONY: test_java
test_java: libgv_java.la
	-(mkdir -p test_java; cd test_java; \
		ln -fs ../.libs/libgv_java.so libgv.so; \
		ln -fs ../$(srcdir)/*.dot ../$(srcdir)/*.java .; \
		javac *.java; \
		LD_LIBRARY_PATH=. CLASSPATH=. java test)

.PHONY: test_lua
test_lua: libgv_lua.la
	-(mkdir -p test_lua; cd test_lua; \
		ln -fs ../.libs/libgv_lua.so gv.so;\
		ln -fs ../$(srcdir)/*.dot ../$(srcdir)/*.lua .; \
		$(LUA) test.lua)

.PHONY: test_ocaml
test_ocaml: libgv_ocaml.la
	-(mkdir -p test_ocaml; cd test_ocaml; \
		ln -fs ../.libs/libgv_ocaml.so libgv.so; \
		ln -fs ../$(srcdir)/*.dot ../$(srcdir)/*.ml ../$(srcdir)/*.mli .; \
		ocamlc *.ocaml; \
		LD_LIBRARY_PATH=. ocaml test)

.PHONY: test_perl
test_perl: libgv_perl.la
	-(mkdir -p test_perl; cd test_perl; \
		ln -fs ../.libs/libgv_perl.so gv.so; \
		ln -fs ../$(srcdir)/*.dot ../$(srcdir)/*.p[lm] .; \
		$(PERL) ./test.pl)

.PHONY: test_php
test_php: libgv_php.la
	-(mkdir -p test_php; cd test_php; \
		ln -fs ../.libs/libgv_php.so libgv.so; \
		ln -fs ../$(srcdir)/*.dot ../$(srcdir)/*.php .; \
		$(PHP) ./test.php)

.PHONY: test_python
test_python: libgv_python.la
	-(mkdir -p test_python; cd test_python; \
		ln -fs ../.libs/libgv_python.so _gv.so; \
		ln -fs ../$(srcdir)/*.dot ../$(srcdir)/*.py .; \
		PYTHONPATH=. $(PYTHON) test.py)

.PHONY: test_R
test_R: libgv_R.la
	-(mkdir -p test_R; cd test_R; \
		ln -fs ../.libs/libgv_R.so gv.so; \
		ln -fs ../$(srcdir)/*.dot ../$(srcdir)/*.R .; \
		$(R) test.R)

.PHONY: test_ruby
test_ruby: libgv_ruby.la
	-(mkdir -p test_ruby; cd test_ruby; \
		ln -fs ../.libs/libgv_ruby.so gv.so; \
		ln -fs ../$(srcdir)/*.dot ../$(srcdir)/*.rb .; \
		$(RUBY) test.rb)

.PHONY: test_tcl
test_tcl: libgv_tcl.la
	-(mkdir -p test_tcl; cd test_tcl; \
		ln -fs ../.libs/libgv_tcl.so libgv_tcl.so; \
		ln -fs ../$(srcdir)/*.dot ../$(srcdir)/*.tcl .; \
		$(TCLSH) ./test.tcl)

EXTRA_DIST = $(man) $(pdf) gv_doc_*.tcl

CLEANFILES = test_*/* pkgIndex.tcl \
	gv_sharp.cpp $(SHARP_data) \
	gv_guile.cpp $(GUILE_data) \
	gv_io.cpp $(IO_data) \
	gv_java.cpp $(JAVA_data) \
	gv_lua.cpp $(LUA_data) \
	gv_ocaml.cpp gv.cma gv.cmi gv.cma gv.cmi gv.a gv.cmx gv.cmxa gv.mli swig.mli swig.ml swig.cmi swig.cmo gv.ml gv.mli gv.cmi gv.cmo gv.cmx gv.cmxa gv.a gv.o gv.cma \
	gv_perl.cpp $(PERL_data) \
	gv_php.cpp $(PHP_data) \
	gv_python.cpp $(PYTHON_data) gv.pyc \
	gv_python23.cpp $(PYTHON23_data) gv.pyc \
	gv_python24.cpp $(PYTHON24_data) gv.pyc \
	gv_python25.cpp $(PYTHON25_data) gv.pyc \
	gv_R.cpp $(R_data) \
	gv_ruby.cpp $(RUBY_data) \
	gv_tcl.cpp $(TCL_data) pkgIndex.tcl

DISTCLEANFILES = gv_*.n gv_*.pdf

