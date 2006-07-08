# $Id$ $Revision$
## Process this file with cmake to produce Makefile

ADD_SUBDIRECTORY( ${sfio_SRCDIR}/Sfio_dc ) 
ADD_SUBDIRECTORY( ${sfio_SRCDIR}/Sfio_f ) 

ADD_CUSTOM_COMMAND(
	OUTPUT ${top_SRCDIR}/FEATURE/sfio
	DEPENDS ${sfio_SRCDIR}/features/sfio
	COMMAND mkdir -p ${top_SRCDIR}/FEATURE
	COMMAND ${top_SRCDIR}/iffe - set cc /usr/bin/gcc : run ${sfio_SRCDIR}/features/sfio > ${top_SRCDIR}/FEATURE/sfio
)

INCLUDE_DIRECTORIES(
	${sfio_SRCDIR}
	${top_SRCDIR}
	${ast_SRCDIR}
)

SET(sfio_SRCS
	${sfio_SRCDIR}/sfclose.c
	${sfio_SRCDIR}/sfclrlock.c
	${sfio_SRCDIR}/sfcvt.c
	${sfio_SRCDIR}/sfdisc.c
	${sfio_SRCDIR}/sfdlen.c
	${sfio_SRCDIR}/sfexcept.c
	${sfio_SRCDIR}/sfexit.c
	${sfio_SRCDIR}/sfextern.c
	${sfio_SRCDIR}/sffcvt.c
	${sfio_SRCDIR}/sffilbuf.c
	${sfio_SRCDIR}/sfflsbuf.c
	${sfio_SRCDIR}/sfgetd.c
	${sfio_SRCDIR}/sfgetl.c
	${sfio_SRCDIR}/sfgetm.c
	${sfio_SRCDIR}/sfgetr.c
	${sfio_SRCDIR}/sfgetu.c
	${sfio_SRCDIR}/sfllen.c
	${sfio_SRCDIR}/sfmode.c
	${sfio_SRCDIR}/sfmove.c
	${sfio_SRCDIR}/sfmutex.c
	${sfio_SRCDIR}/sfnew.c
	${sfio_SRCDIR}/sfnotify.c
	${sfio_SRCDIR}/sfnputc.c
	${sfio_SRCDIR}/sfopen.c
	${sfio_SRCDIR}/sfpkrd.c
	${sfio_SRCDIR}/sfpoll.c
	${sfio_SRCDIR}/sfpool.c
	${sfio_SRCDIR}/sfpopen.c
	${sfio_SRCDIR}/sfprintf.c
	${sfio_SRCDIR}/sfprints.c
	${sfio_SRCDIR}/sfpurge.c
	${sfio_SRCDIR}/sfputd.c
	${sfio_SRCDIR}/sfputl.c
	${sfio_SRCDIR}/sfputm.c
	${sfio_SRCDIR}/sfputr.c
	${sfio_SRCDIR}/sfputu.c
	${sfio_SRCDIR}/sfraise.c
	${sfio_SRCDIR}/sfrd.c
	${sfio_SRCDIR}/sfread.c
	${sfio_SRCDIR}/sfreserve.c
	${sfio_SRCDIR}/sfresize.c
	${sfio_SRCDIR}/sfscanf.c
	${sfio_SRCDIR}/sfseek.c
	${sfio_SRCDIR}/sfset.c
	${sfio_SRCDIR}/sfsetbuf.c
	${sfio_SRCDIR}/sfsetfd.c
	${sfio_SRCDIR}/sfsize.c
	${sfio_SRCDIR}/sfsk.c
	${sfio_SRCDIR}/sfstack.c
	${sfio_SRCDIR}/sfstrtod.c
	${sfio_SRCDIR}/sfswap.c
	${sfio_SRCDIR}/sfsync.c
	${sfio_SRCDIR}/sftable.c
	${sfio_SRCDIR}/sftell.c
	${sfio_SRCDIR}/sftmp.c
	${sfio_SRCDIR}/sfungetc.c
	${sfio_SRCDIR}/sfvprintf.c
	${sfio_SRCDIR}/sfvscanf.c
	${sfio_SRCDIR}/sfwr.c
	${sfio_SRCDIR}/sfwrite.c
)
