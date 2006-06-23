# $Id$ $Revision$
## Process this file with cmake to produce Makefile

ADD_SUBDIRECTORY( x11 ) 
ADD_SUBDIRECTORY( mswin32 ) 
ADD_SUBDIRECTORY( none ) 


########### install files ###############




#original Makefile.am contents follow:

## $Id$ $Revision$
### Process this file with automake to produce Makefile.in
#
#SUBDIRS = x11
#
#EXTRA_DIST = mswin32 gtk none
