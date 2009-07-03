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

#ifdef WIN32
#include "windows.h"
#include "Shlwapi.h"
#endif

#include <gprstate.h>
#include <error.h>
#include <sfstr.h>

int validTVT(int c)
{
    return ((TV_flat <= c) && (c <= TV_prepostrev));
}

void initGPRState(Gpr_t * state, Vmalloc_t * vm)
{
    state->tgtname = vmstrdup(vm, "gvpr_result");
}

Gpr_t *openGPRState(gpr_info* info)
{
    Gpr_t *state;

    if (!(state = newof(0, Gpr_t, 1, 0))) {
	error(ERROR_ERROR, "Could not create gvpr state: out of memory");
	return state;
    }

    if (!(state->tmp = sfstropen())) {
	error(ERROR_ERROR, "Could not create state tmpfile");
	free (state);
	return 0;
    }

    state->tvt = TV_flat;
    state->tvroot = 0;
    state->tvedge = 0;
    state->outFile = info->outFile;
    state->argc = info->argc;
    state->argv = info->argv;
    state->errf = info->errf;
    state->flags = info->flags;

    return state;
}

void closeGPRState(Gpr_t* state)
{
    if (!state) return;
    if (state->tmp)
	sfclose (state->tmp);
    free (state->dp);
    free (state);
}

#ifdef WIN32_DLL
int pathisrelative (char* path)
{
    return PathIsRelative(path);

}
#endif

