# $Id$ $Revision$
## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = \
        -I$(top_srcdir) \
        -I$(top_srcdir)/lib/agraph \
        -I$(top_srcdir)/lib/cdt

if WITH_CGRAPH
else
noinst_HEADERS = agutil.h
noinst_LTLIBRARIES = libagutil_C.la
endif

libagutil_C_la_SOURCES = dynattr.c nodeq.c agutil.h
