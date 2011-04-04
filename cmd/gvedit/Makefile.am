# $Id$ $Revision$
## Process this file with automake to produce Makefile.in

SUBDIRS = ui images

gveditdir = $(pkgdatadir)/gvedit

if WITH_QT
bin_PROGRAMS = gvedit
gvedit_DATA = attrs.txt
endif

gvedit_SOURCES = main.cpp csettings.cpp mainwindow.cpp imageviewer.cpp mdichild.cpp

noinst_HEADERS = csettings.h imageviewer.h mainwindow.h mdichild.h ui_settings.h 

qMakefile: gvedit.pro
	qmake-qt4 -o qMakefile gvedit.pro

gvedit: main.cpp csettings.cpp mainwindow.cpp imageviewer.cpp mdichild.cpp qMakefile
	$(MAKE) -f qMakefile prefix=${prefix} exec_prefix=${exec_prefix}

attrs.txt: $(top_srcdir)/doc/infosrc/attrs
	cp $(top_srcdir)/doc/infosrc/attrs attrs.txt

EXTRA_DIST = \
	gvedit.pro.in gvedit.ncb gvedit.sln gvedit.vcproj gvedit.vcproj.RESEARCH.arif.user gvedit.vcproj.user \
	mdi.pro mdi.qrc mdi.vcproj mdi.vcproj.RESEARCH.arif.user \
	document1.png document2.png vc90.idb vc90.pdb

DISTCLEAN = attrs.txt
