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


/*	Set the control flags for a region.
**
**	Written by Kiem-Phong Vo, kpv@research.att.com, 01/16/94.
*/
#if __STD_C
int vmset(reg Vmalloc_t * vm, int flags, int on)
#else
int vmset(vm, flags, on)
reg Vmalloc_t *vm;		/* region being worked on               */
int flags;			/* flags must be in VM_FLAGS            */
int on;				/* !=0 if turning on, else turning off  */
#endif
{
    reg int mode;
    reg Vmdata_t *vd = vm->data;

    if (flags == 0 && on == 0)
	return vd->mode;

    if (!(vd->mode & VM_TRUST)) {
	if (ISLOCK(vd, 0))
	    return 0;
	SETLOCK(vd, 0);
    }

    mode = vd->mode;

    if (on)
	vd->mode |= (flags & VM_FLAGS);
    else
	vd->mode &= ~(flags & VM_FLAGS);

    if (vd->mode & (VM_TRACE | VM_MTDEBUG))
	vd->mode &= ~VM_TRUST;

    CLRLOCK(vd, 0);

    return mode;
}
