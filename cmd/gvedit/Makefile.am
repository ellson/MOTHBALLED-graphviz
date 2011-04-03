# $Id$ $Revision$
## Process this file with automake to produce Makefile.in

SUBDIRS = ui images

all: gvedit

qMakefile: gvedit.pro
	qmake-qt4 -o qMakefile gvedit.pro

gvedit: main.cpp qMakefile
	$(MAKE) -f qMakefile prefix=${prefix} exec_prefix=${exec_prefix}

EXTRA_DIST = csettings.cpp csettings.h document1.png document2.png \
	gvedit.pro gvedit.ncb gvedit.sln gvedit.vcproj gvedit.vcproj.RESEARCH.arif.user gvedit.vcproj.user \
	imageviewer.cpp imageviewer.h \
	main.cpp mainwindow.cpp mainwindow.h \
	mdi.pro mdi.qrc mdi.vcproj mdi.vcproj.RESEARCH.arif.user mdichild.cpp mdichild.h \
	ui_settings.h vc90.idb vc90.pdb

