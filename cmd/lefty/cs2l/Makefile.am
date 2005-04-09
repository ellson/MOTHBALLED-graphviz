## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = -I$(top_srcdir)/cmd/lefty -DHAVE_CONFIG_H \
        -DFEATURE_MINTSIZE -DFEATURE_DOT -DFEATURE_X11

noinst_HEADERS = cs2l.h
noinst_LTLIBRARIES = libcs2l.la

libcs2l_la_SOURCES = cs2l.c

EXTRA_DIST = cs2l.c
