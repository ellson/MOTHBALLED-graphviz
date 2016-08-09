# $Id$ $Revision$
## Process this file with automake to produce Makefile.in

if HAVE_CRITERION

AM_CPPFLAGS = \
	-I$(top_srcdir) \
    -I$(top_srcdir)/lib/common \
    -I$(top_srcdir)/lib/gvc \
	-I$(top_srcdir)/lib/pathplan \
	-I$(top_srcdir)/lib/cgraph \
	-I$(top_srcdir)/lib/cdt \
    -DDEMAND_LOADING=1

AM_LDFLAGS = \
	-lcriterion

TESTS = command_line

bin_PROGRAMS = $(TESTS)

command_line_SOURCES = command_line.c
command_line_CPPFLAGS = $(AM_CPPFLAGS) -DDEMAND_LOADING=1
command_line_LDADD = \
	$(top_builddir)/lib/gvc/libgvc.la

endif
