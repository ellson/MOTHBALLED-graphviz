## Process this file with automake to produce Makefile.in

SUBDIRS = libfilereq

AM_CPPFLAGS = -I$(top_srcdir)/lefty @XAW_INCLUDES@

AM_CFLAGS =  @X_CFLAGS@

noinst_HEADERS = gcommon.h
if WITH_X
noinst_LTLIBRARIES = libws.la
endif

libws_la_SOURCES = garray.c gbutton.c gcanvas.c gcommon.c glabel.c \
	gmenu.c gpcanvas.c gquery.c gscroll.c gtext.c gview.c

EXTRA_DIST = $(libws_la_SOURCES)
