/* $Id$ $Revision$ */
/* vim:set shiftwidth=4 ts=8: */

/**********************************************************
*      This software is part of the graphviz package      *
*                http://www.graphviz.org/                 *
*                                                         *
*            Copyright (c) 1994-2004 AT&T Corp.           *
*                and is licensed under the                *
*            Common Public License, Version 1.0           *
*                      by AT&T Corp.                      *
*                                                         *
*        Information and Software Systems Research        *
*              AT&T Research, Florham Park NJ             *
**********************************************************/

#include	"vmhdr.h"

/*
**	Any required functions for process exiting.
**	Written by Kiem-Phong Vo, kpv@research.att.com (05/25/93).
*/
#if _PACKAGE_ast || _lib_atexit
int Vm_atexit_already_defined;
#else

#if _lib_onexit

#if __STD_C
int atexit(void (*exitf) (void))
#else
int atexit(exitf)
void (*exitf) ();
#endif
{
    return onexit(exitf);
}

#else				/*!_lib_onexit */

typedef struct _exit_s {
    struct _exit_s *next;
    void (*exitf) _ARG_((void));
} Exit_t;
static Exit_t *Exit;

#if __STD_C
atexit(void (*exitf) (void))
#else
atexit(exitf)
void (*exitf) ();
#endif
{
    Exit_t *e;

    if (!(e = (Exit_t *) malloc(sizeof(Exit_t))))
	return -1;
    e->exitf = exitf;
    e->next = Exit;
    Exit = e;
    return 0;
}

#if __STD_C
void exit(int type)
#else
void exit(type)
int type;
#endif
{
    Exit_t *e;

    for (e = Exit; e; e = e->next)
	(*e->exitf) ();

#if _exit_cleanup
    _cleanup();
#endif

    _exit(type);
}

#endif				/* _lib_onexit || _lib_on_exit */

#endif				/*!PACKAGE_ast */
