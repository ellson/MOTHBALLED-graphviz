# $Id$ $Revision$
# ## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/lib/common \
	-I$(top_srcdir)/lib/cgraph \
	-I$(top_srcdir)/lib/cdt

if WITH_SMYRNA
noinst_HEADERS = xdot.h
noinst_LTLIBRARIES = libxdot_C.la
endif

libxdot_C_la_SOURCES = xdot.c
