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

/*	Get the segment containing this address
**
**	Written by Kiem-Phong Vo, kpv@research.att.com, 02/07/95
*/

#if __STD_C
Void_t *vmsegment(Vmalloc_t * vm, Void_t * addr)
#else
Void_t *vmsegment(vm, addr)
Vmalloc_t *vm;			/* region       */
Void_t *addr;			/* address      */
#endif
{
    reg Seg_t *seg;
    reg Vmdata_t *vd = vm->data;

    if (!(vd->mode & VM_TRUST)) {
	if (ISLOCK(vd, 0))
	    return NIL(Void_t *);
	SETLOCK(vd, 0);
    }

    for (seg = vd->seg; seg; seg = seg->next)
	if ((Vmuchar_t *) addr >= (Vmuchar_t *) seg->addr &&
	    (Vmuchar_t *) addr < (Vmuchar_t *) seg->baddr)
	    break;

    CLRLOCK(vd, 0);
    return seg ? (Void_t *) seg->addr : NIL(Void_t *);
}
