## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = -I$(top_srcdir)/lib/sfio

noinst_HEADERS = sfdchdr.h sfdisc.h
noinst_LTLIBRARIES = libsfiodc.la

libsfiodc_la_SOURCES = sfdcdio.c sfdcdos.c sfdcfilter.c sfdclzw.c \
	sfdcseekable.c sfdcslow.c sfdcsubstream.c sfdctee.c sfdcunion.c

${top_builddir}/FEATURE/sfio:  ${top_srcdir}/lib/sfio/features/sfio
	mkdir -p ${top_builddir}/FEATURE
	@KSH@ $(top_srcdir)/iffe - set cc $(CC) $(CCMODE) $(CXFLAGS) : run  ${top_srcdir}/lib/sfio/features/sfio > $@

$(libsfiodc_la_OBJECTS): ${top_builddir}/FEATURE/sfio

DISTCLEANFILES = ${top_builddir}/FEATURE/sfio

EXTRA_DIST = Makefile.old
