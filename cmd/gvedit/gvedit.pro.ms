# Rename gvedit.pro, run 'qmake -tp vc' to create gvedit.vcproj file
LIBS += \
        -LC:/gtk/lib \
        -LC:/graphviz-ms/bin \
        -lgvc -lcgraph -lcdt -lexpat -lz -lltdl

INCLUDEPATH += \
	../../lib/gvc \
	../../lib/common \
	../../lib/pathplan \
	../../lib/cgraph \
	../../lib/cdt \
	../..

CONFIG += qt
HEADERS = mainwindow.h mdichild.h csettings.h imageviewer.h ui_settings.h
SOURCES = main.cpp mainwindow.cpp mdichild.cpp csettings.cpp imageviewer.cpp
RESOURCES     = mdi.qrc

