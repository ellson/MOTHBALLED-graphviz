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
 *	dllEntry.c
 */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN

/*
 * The following declarations are for the Borland and VC++
 * DLL entry points.
 */

BOOL APIENTRY DllEntryPoint(HINSTANCE hInst,
			    DWORD reason, LPVOID reserved);
BOOL APIENTRY DllMain(HINSTANCE hInst, DWORD reason, LPVOID reserved);


/*
 *----------------------------------------------------------------------
 *
 * DllEntryPoint --
 *
 *	This routine is called by Borland to invoke the
 *	initialization code for the library.
 *
 * Results:
 *	TRUE.
 *
 * Side effects:
 *	See DllMain.
 *
 *----------------------------------------------------------------------
 */

BOOL APIENTRY DllEntryPoint(HINSTANCE hInst,	/* Library instance handle. */
			    DWORD reason,	/* Reason this function is being called. */
			    LPVOID reserved)
{				/* Not used. */
    return TRUE;
}

/*
 *----------------------------------------------------------------------
 *
 * DllMain --
 *
 *	This routine is called by the VC++ C run time library init
 *	code invoke the initialization code for the library.
 *
 * Results:
 *	TRUE.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

BOOL APIENTRY DllMain(HINSTANCE hInst,	/* Library instance handle. */
		      DWORD reason,	/* Reason this function is being called. */
		      LPVOID reserved)
{				/* Not used. */
    return TRUE;
}
