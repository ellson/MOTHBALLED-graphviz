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

#include "sfdchdr.h"

/*	Make a stream op return immediately on interrupts.
**	This is useful on slow streams (hence the name).
**
**	Written by Glenn Fowler (03/18/1998).
*/

static int slowexcept(Sfio_t * f, int type, void * v, Sfdisc_t * disc)
{
    NOTUSED(f);
    NOTUSED(v);
    NOTUSED(disc);

    switch (type) {
    case SF_FINAL:
    case SF_DPOP:
	free(disc);
	break;
    case SF_READ:
    case SF_WRITE:
	if (errno == EINTR)
	    return (-1);
	break;
    }

    return (0);
}

int sfdcslow(Sfio_t * f)
{
    Sfdisc_t *disc;

    if (!(disc = (Sfdisc_t *) malloc(sizeof(Sfdisc_t))))
	return (-1);

    disc->readf = NIL(Sfread_f);
    disc->writef = NIL(Sfwrite_f);
    disc->seekf = NIL(Sfseek_f);
    disc->exceptf = slowexcept;

    if (sfdisc(f, disc) != disc) {
	free(disc);
	return (-1);
    }

    return (0);
}
