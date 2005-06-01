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


/*
 * gpr state
 *
 */

#include <gprstate.h>
#include <error.h>
#include <sfstr.h>

int validTVT(int c)
{
    int rv = 0;

    switch (c) {
    case TV_flat:
    case TV_bfs:
    case TV_dfs:
    case TV_fwd:
    case TV_rev:
    case TV_ne:
    case TV_en:
	rv = 1;
	break;
    }
    return rv;
}

void initGPRState(Gpr_t * state, Vmalloc_t * vm, gpr_info * info)
{
    state->tgtname = vmstrdup(vm, "gvpr_result");
    state->tvt = TV_flat;
    state->tvroot = 0;
    state->outFile = info->outFile;
    state->argc = info->argc;
    state->argv = info->argv;
}

Gpr_t *openGPRState()
{
    Gpr_t *state;

    if (!(state = newof(0, Gpr_t, 1, 0)))
	error(ERROR_FATAL, "Could not create gvpr state: out of memory");

    if (!(state->tmp = sfstropen()))
	error(ERROR_FATAL, "Could not create state");

    return state;
}
