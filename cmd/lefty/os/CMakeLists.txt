# $Id$ $Revision$
## Process this file with cmake to produce Makefile

ADD_SUBDIRECTORY( unix ) 
ADD_SUBDIRECTORY( mswin32 ) 


#original Makefile.am contents follow:

## $Id$ $Revision$
### Process this file with automake to produce Makefile.in
#
#SUBDIRS = unix
#
#EXTRA_DIST = mswin32/lefty.mak mswin32/doinst mswin32/io.c
