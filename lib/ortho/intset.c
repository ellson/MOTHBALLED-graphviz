/* $Id$Revision: */
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stddef.h>
#include <intset.h>
#include <memory.h>

static Void_t*
mkIntItem(Dt_t* d,intitem* obj,Dtdisc_t* disc)
{ 
    intitem* np = NEW(intitem);
    np->id = obj->id;
    return (Void_t*)np;
}

static void
freeIntItem(Dt_t* d,intitem* obj,Dtdisc_t* disc)
{
    free (obj);
}

static int
cmpid(Dt_t* d, int* key1, int* key2, Dtdisc_t* disc)
{
  if (*key1 > *key2) return 1;
  else if (*key1 < *key2) return -1;
  else return 0;
}   

static Dtdisc_t intSetDisc = {
    offsetof(intitem,id),
    sizeof(int),
    offsetof(intitem,link),
    (Dtmake_f)mkIntItem,
    (Dtfree_f)freeIntItem,
    (Dtcompar_f)cmpid,
    0,
    0,
    0
};

Dt_t* 
openIntSet ()
{
	return dtopen(&intSetDisc,Dtoset);
}

