# $Id$ $Revision$
## Process this file with cmake to produce Makefile

PROJECT(graphviz)

SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

CONFIGURE_FILE(
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake/uninstall.cmake.in"
	"${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
	IMMEDIATE @ONLY
)

# create config.h
#INCLUDE (ConfigureChecks.cmake)
#CONFIGURE_FILE (config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h )

ADD_DEFINITIONS( -DHAVE_CONFIG_H )

ADD_SUBDIRECTORY(lib)
ADD_SUBDIRECTORY(plugin)
ADD_SUBDIRECTORY(cmd)
ADD_SUBDIRECTORY(tclpkg)
ADD_SUBDIRECTORY(doc)
ADD_SUBDIRECTORY(contrib)
ADD_SUBDIRECTORY(graphs)

ADD_CUSTOM_TARGET(uninstall
	"${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
)

# FIXME - this config.h comes from ./configure
ADD_CUSTOM_COMMAND(
	OUTPUT ${CMAKE_BINARY_DIR}/config.h
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/config.h
	COMMAND cat < ${CMAKE_CURRENT_SOURCE_DIR}/config.h > ${CMAKE_BINARY_DIR}/config.h
)

# FIXME - this ast_common.h comes from ./configure
ADD_CUSTOM_COMMAND(
	OUTPUT ${CMAKE_BINARY_DIR}/ast_common.h
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/ast_common.h
	COMMAND cat < ${CMAKE_CURRENT_SOURCE_DIR}/ast_common.h > ${CMAKE_BINARY_DIR}/ast_common.h
)


########### install files ###############

INSTALL_FILES(FILES
	AUTHORS
	COPYING
	ChangeLog
	NEWS
	cpl1.0.txt
)



#original Makefile.am contents follow:

## $Id$ $Revision$
### Process this file with automake to produce Makefile.in
#
#ACLOCAL_AMFLAGS = -I m4
#
#txtdir = $(pkgdatadir)/doc
#htmldir = $(pkgdatadir)/doc/html
#
#txt = AUTHORS COPYING ChangeLog NEWS cpl1.0.txt
#html =
#
#txt_DATA = $(txt)
#html_DATA = $(html)
#
## $(subdirs) contains the list from: AC_CONFIG_SUBDIRS
#SUBDIRS = $(subdirs) lib cmd tclpkg doc contrib graphs
#
#.PHONY: doxygen
#doxygen:
#	doxygen
#
#dist-hook:
#	find $(distdir) -name CVS -type d -prune -exec rm -rf {} \;
##	rm -f ToPAS
##	find $(distdir) -type f -exec md5sum {} >>ToPAS \;
#
#EXTRA_DIST = $(txt) $(html) graphviz.spec.in graphviz.spec \
#	autogen.sh config/depcomp config/config.rpath \
#	builddate.h ast_common.h.in \
#	Makefile.old Config.mk.old configure.old INSTALL.old \
#	Makeargs config.h.old compat_getopt.h iffe config.iffe \
#	m4/README m4/iconv.m4 m4/lib-ld.m4 m4/lib-link.m4 m4/lib-prefix.m4 \
#	Doxyfile.in Doxyfile \
#	makearch features awk windows \
#	config/config_perl.pl config/config_python.py \
#	config/config_ruby.rb config/config_tcl.tcl dot.demo
#
#.PHONY: rpm
#rpm: dist
#	rpmbuild -ta graphviz-@VERSION@.tar.gz
#
#DISTCLEANFILES = config.cache builddate.h ast_common.h FEATURE/* graphviz-@VERSION@*
