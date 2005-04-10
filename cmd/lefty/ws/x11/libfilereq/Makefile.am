## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = @X_CFLAGS@ \
	$(XAW_INCLUDES)

noinst_HEADERS = SFinternal.h SFDecls.h xstat.h SF.h
if WITH_X
noinst_LTLIBRARIES = libfilereq.la
endif

libfilereq_la_SOURCES = Dir.c Draw.c Path.c SelFile.c
libfilereq_la_LIBADD = $(XAW_LIBS)

EXTRA_DIST = README.selfile
