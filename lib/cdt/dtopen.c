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

#include	"dthdr.h"

#ifdef DMALLOC
#include "dmalloc.h"
#endif

static char *Version = "\n@(#)cdt (AT&T Labs - kpv) 1999-11-01\0\n";

/* 	Make a new dictionary
**
**	Written by Kiem-Phong Vo (5/25/96)
*/

#if __STD_C
Dt_t *dtopen(Dtdisc_t * disc, Dtmethod_t * meth)
#else
Dt_t *dtopen(disc, meth)
Dtdisc_t *disc;
Dtmethod_t *meth;
#endif
{
    Dt_t *dt = (Dt_t *) Version;	/* shut-up unuse warning */
    reg int e;
    Dtdata_t *data;

    if (!disc || !meth)
	return NIL(Dt_t *);

    /* allocate space for dictionary */
    if (!(dt = (Dt_t *) malloc(sizeof(Dt_t))))
	return NIL(Dt_t *);

    /* initialize all absolutely private data */
    dt->searchf = NIL(Dtsearch_f);
    dt->meth = NIL(Dtmethod_t *);
    dt->disc = NIL(Dtdisc_t *);
    dtdisc(dt, disc, 0);
    dt->nview = 0;
    dt->view = dt->walk = NIL(Dt_t *);

    if (disc->eventf) {		/* if shared/persistent dictionary, get existing data */
	data = NIL(Dtdata_t *);
	if ((e =
	     (*disc->eventf) (dt, DT_OPEN, (Void_t *) (&data),
			      disc)) != 0) {
	    if (e < 0 || !data || !(data->type & meth->type)) {
		free((Void_t *) dt);
		return NIL(Dt_t *);
	    } else
		goto done;
	}
    }

    /* allocate sharable data */
    data =
	(Dtdata_t *) (dt->memoryf) (dt, NIL(Void_t *), sizeof(Dtdata_t),
				    disc);
    if (!data) {
	free((Void_t *) dt);
	return NIL(Dt_t *);
    }
    data->type = meth->type;
    data->here = NIL(Dtlink_t *);
    data->htab = NIL(Dtlink_t **);
    data->ntab = data->size = data->loop = 0;

  done:
    dt->data = data;
    dt->searchf = meth->searchf;
    dt->meth = meth;

    return dt;
}

#if __hppa

/*
 * some dll implementations forget that data symbols
 * need address resolution too
 */

#if __STD_C
int _dt_dynamic_data(void)
#else
int _dt_dynamic_data()
#endif
{
    return (Dtset != 0) + (Dtlist != 0) + (Dttree != 0);
}

#endif				/* __hppa */
