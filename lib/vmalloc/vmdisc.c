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

/*	Change the discipline for a region.  The old discipline
**	is returned.  If the new discipline is NIL then the
**	discipline is not changed.
**
**	Written by Kiem-Phong Vo, kpv@research.att.com, 01/16/94.
*/
#if __STD_C
Vmdisc_t *vmdisc(Vmalloc_t * vm, Vmdisc_t * disc)
#else
Vmdisc_t *vmdisc(vm, disc)
Vmalloc_t *vm;
Vmdisc_t *disc;
#endif
{
    Vmdisc_t *old = vm->disc;

    if (disc) {
	if (disc->memoryf != old->memoryf)
	    return NIL(Vmdisc_t *);
	if (old->exceptf &&
	    (*old->exceptf) (vm, VM_DISC, (Void_t *) disc, old) != 0)
	    return NIL(Vmdisc_t *);
	vm->disc = disc;
    }
    return old;
}
