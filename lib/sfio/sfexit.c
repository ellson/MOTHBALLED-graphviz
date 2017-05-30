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

#include	"sfhdr.h"

/*
**	Any required functions for process exiting.
**	Written by Kiem-Phong Vo
*/

#ifdef HAVE_WAITPID
int _Sf_no_need_for_waitpid;
#else

/* we need to supply our own waitpid here so that sfpclose() can wait
** for the right process to die.
*/
typedef struct _wait_ {
    int pid;
    int status;
    struct _wait_ *next;
} Waitpid_t;

static Waitpid_t *Wait;

#ifndef _WIN32
waitpid(int pid, int *status, int options)
{
    int id, ps;
    Waitpid_t *w;
    Waitpid_t *last;

    /* we don't know options */
    if (options != 0)
	return -1;

    vtmtxlock(_Sfmutex);

    for (w = Wait, last = NIL(Waitpid_t *); w; last = w, w = w->next) {
	if (pid > 0 && pid != w->pid)
	    continue;

	if (last)
	    last->next = w->next;
	else
	    Wait = w->next;
	if (status)
	    *status = w->status;
	pid = w->pid;
	free(w);

	vtmtxunlock(_Sfmutex);
	return pid;
    }

    while ((id = wait(&ps)) >= 0) {
	if (pid <= 0 || id == pid) {
	    if (status)
		*status = ps;

	    vtmtxunlock(_Sfmutex);
	    return pid;
	}

	if (!(w = (Waitpid_t *) malloc(sizeof(Waitpid_t))))
	    continue;

	w->pid = id;
	w->status = ps;
	w->next = Wait;
	Wait = w;
    }

    vtmtxunlock(_Sfmutex);
    return -1;
}

#endif /*HAVE_WAITPID*/

#endif
