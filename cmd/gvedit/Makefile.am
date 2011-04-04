# $Id$ $Revision$
## Process this file with automake to produce Makefile.in

SUBDIRS = ui images

pdfdir = $(pkgdatadir)/doc/pdf
gveditdir = $(pkgdatadir)/gvedit

if WITH_QT
bin_PROGRAMS = gvedit
gvedit_DATA = attrs.txt
endif

man_MANS = gvedit.1
pdf_DATA = gvedit.1.pdf

gvedit_SOURCES = main.cpp csettings.cpp mainwindow.cpp imageviewer.cpp mdichild.cpp

noinst_HEADERS = csettings.h imageviewer.h mainwindow.h mdichild.h ui_settings.h 

qMakefile: gvedit.pro
	$(QMAKE) -o qMakefile gvedit.pro

gvedit: main.cpp csettings.cpp mainwindow.cpp imageviewer.cpp mdichild.cpp qMakefile
	$(MAKE) -f qMakefile prefix=${prefix} exec_prefix=${exec_prefix} top_builddir=$(top_builddir)
	- $(LN_S) gvedit.app/Contents/MacOS/gvedit .

attrs.txt: $(top_srcdir)/doc/infosrc/attrs
	cp $(top_srcdir)/doc/infosrc/attrs attrs.txt

gvedit.1.pdf: $(srcdir)/gvedit.1
	- $(GROFF) -Tps -man $(srcdir)/gvedit.1 | @PS2PDF@ - - >gvedit.1.pdf

EXTRA_DIST = gvedit.pro.in mdi.qrc $(man_MANS) $(pdf_DATA)
	
DISTCLEANFILES = attrs.txt $(pdf_DATA)

clean-local:
	-$(MAKE) -f qMakefile clean

distclean-local:
	-$(MAKE) -f qMakefile distclean

