/* $Id$ $Revision$ */
/* vim:set shiftwidth=4 ts=8: */

/*************************************************************************
 * Copyright (c) 2011 AT&T Intellectual Property 
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors: See CVS logs. Details at http://www.graphviz.org/
 *************************************************************************/

#include	"vmhdr.h"

/*	A discipline to get memory from the heap.
**
**	Written by Kiem-Phong Vo, kpv@research.att.com, 01/16/94.
*/
/**
 * @param vm region doing allocation from
 * @param caddr current low address
 * @param csize current size
 * @param nsize new size
 * @param disc discipline structure
 */
static void *heapmem(Vmalloc_t * vm, void * caddr,
		       size_t csize, size_t nsize, Vmdisc_t * disc)
{
    NOTUSED(vm);
    NOTUSED(disc);

    if (csize == 0)
	return vmalloc(Vmheap, nsize);
    else if (nsize == 0)
	return vmfree(Vmheap, caddr) >= 0 ? caddr : NIL(void *);
    else
	return vmresize(Vmheap, caddr, nsize, 0);
}

static Vmdisc_t _Vmdcheap = { heapmem, NIL(Vmexcept_f), 0 };

Vmdisc_t* Vmdcheap = &_Vmdcheap;
