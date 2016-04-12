AUTOMAKE_OPTIONS = foreign no-dependencies
lib_LTLIBRARIES = librxspencer.la
librxspencer_la_LDFLAGS = -version-info 0:2:0 -no-undefined -module
AM_CPPFLAGS = -DPOSIX_MISTAKE
librxspencer_la_SOURCES = regcomp.c regerror.c regexec.c regfree.c
noinst_HEADERS = cclass.h cname.h engine.c regex2.h utils.h
man_MANS = rxspencer.3 rxspencer.7
BUILT_SOURCES = main.ih debug.ih regerror.ih engine.ih regcomp.ih regex.h
test_sources = split.c debug.c main.c 
re_oo_objs = split.oo debug.oo main.oo regcomp.oo regerror.oo regexec.oo regfree.oo
re: $(BUILT_SOURCES) $(re_oo_objs)
	$(LINK) -o re $(re_oo_objs)
SUFFIXES = .oo .ih
MKHFLAGS =
.c.oo:
	$(COMPILE) -c -DREDEBUG $< -o $@
.c.ih:
	sh $(srcdir)/mkh $(MKHFLAGS) -p $< > $@
REGEXHSRC = $(srcdir)/regex2.h $(srcdir)/regcomp.c $(srcdir)/regerror.c\
	$(srcdir)/regexec.c $(srcdir)/regfree.c
regex.h: $(REGEXHSRC) mkh
	sh $(srcdir)/mkh $(MKHFLAGS) -i _REGEX_H_ $(REGEXHSRC) > regex.h
install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(pkgincludedir)
	$(INSTALL_DATA) regex.h $(DESTDIR)$(pkgincludedir)
CLEANFILES = re *.oo *.ih regex.h
TESTS = test1.sh test2.sh test3.sh
test1.sh: re
EXTRA_DIST = COPYRIGHT WHATSNEW tests autogen.sh compile mkh\
	Makefile.orig $(TESTS) $(test_sources) $(man_MANS)
debug.lo: debug.c debug.ih regex.h utils.h regex2.h
main.lo: main.c main.ih regex.h 
regcomp.lo: regcomp.c regcomp.ih cclass.h cname.h regex.h regex2.h utils.h 
regerror.lo: regerror.c regerror.ih regex.h utils.h
regexec.lo: regexec.c engine.c engine.ih regex.h regex2.h utils.h
split.lo: split.c
