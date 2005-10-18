## Process this file with automake to produce Makefile.in

pdfdir = $(pkgdatadir)/doc/pdf

AM_CPPFLAGS = \
        -I$(top_srcdir) \
        -I$(top_srcdir)/lib/vmalloc \
	-I$(top_srcdir)/lib/sfio \
	-I$(top_srcdir)/lib/ast \
	-I$(top_srcdir)/lib/cdt

pkginclude_HEADERS = exgram.h exlib.h expr.h
pkglib_LTLIBRARIES = libexpr.la
man_MANS = expr.3
pdf_DATA = expr.pdf

libexpr_la_LDFLAGS = -version-info @VERSION_INFO@ -no-undefined

libexpr_la_SOURCES = excc.c excontext.c exdata.c exerror.c \
	exeval.c exexpr.c exlexname.c exopen.c exrewind.c extoken.c \
	extype.c exzero.c exparse.y

libexpr_la_LIBADD = \
	$(top_builddir)/lib/cdt/libcdt.la \
	$(top_builddir)/lib/ast/libast.la \
	$(top_builddir)/lib/sfio/libsfio.la \
	$(top_builddir)/lib/vmalloc/libvmalloc.la -lc

$(libexpr_la_OBJECTS): exparse.h exparse.c exgram.h exop.h

y.output: $(top_srcdir)/lib/expr/exparse.y
	@YACC@ -dtv $(top_srcdir)/lib/expr/exparse.y

exparse.c: y.output 
	@SED@ "s/yy/ex/g" <y.tab.c | \
		@SED@ "s/YY/EX/g" | \
		@SED@ "s/fprintf/sfprintf/g" | \
		@SED@ "s/FILE/Sfio_t/g" | \
		@SED@ "s/stderr/sfstderr/g" > exparse.c

exparse.h: y.output
	echo "#ifndef _EXPARSE_H" > exparse.h
	echo "#define _EXPARSE_H" >> exparse.h
	@SED@ "s/yy/ex/g" < y.tab.h | @SED@ "s/YY/EX/g" >> exparse.h
	echo "#endif /* _EXPARSE_H */" >> exparse.h

exop.h: exparse.h
	echo "static const char* exop[] = {" > exop.h
	echo "	\"MINTOKEN\"," >> exop.h
	$(SED) -e '1,/MINTOKEN/d' -e '/^[ 	]*#[ 	]*define[ 	][ 	]*[A-Z]/!d' -e 's/^[ 	]*#[ 	]*define[ 	]*\([A-Z0-9_]*\).*/	"\1",/' < exparse.h >> exop.h
	echo "};" >> exop.h

.3.pdf:
	groff -Tps -man $< | ps2pdf - - >$@

EXTRA_DIST = $(man_MANS) $(pdf_DATA) RELEASE \
	Makefile.nmake Makefile.orig Makefile.old \
	exparse.c exparse.h exop.h y.output

DISTCLEANFILES = $(pdf_DATA) exparse.[ch] exop.h y.tab.[ch] y.output
