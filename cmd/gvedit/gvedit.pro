
DEFINES += HAVE_CONFIG_H GVEDIT_DATADIR=/Users/erg/share/graphviz/gvedit
LIBS += -L/Users/erg/lib -lgvc -lgraph -lcdt
INCLUDEPATH += ../../lib/gvc ../../lib/common ../../lib/pathplan ../../lib/graph ../../lib/cdt ../..

CONFIG += qt
HEADERS = mainwindow.h mdichild.h csettings.h imageviewer.h ui_settings.h
SOURCES = main.cpp mainwindow.cpp mdichild.cpp csettings.cpp imageviewer.cpp
RESOURCES     = mdi.qrc

