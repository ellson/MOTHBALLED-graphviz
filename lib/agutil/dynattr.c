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

#ifdef WITH_CGRAPH
#include <cgraph.h>
#else
#include <agraph.h>
#endif

#include <agutil.h>
#include <stdlib.h>

double ag_scan_float(void *obj, char *name, double low,
		     double high, double defval)
{
    char *s;
    double rv;

    s = agget(obj, name);
    if (s[0]) {
	rv = atof(s);
	if ((rv < low) || (rv > high))
	    rv = defval;
    } else
	rv = defval;
    return rv;
}

int ag_scan_int(void *obj, char *name, int low, int high, int defval)
{
    char *s;
    int rv;

    s = agget(obj, name);
    if (s[0]) {
	rv = atoi(s);
	if ((rv < low) || (rv > high))
	    rv = defval;
    } else
	rv = defval;
    return rv;
}

#ifdef NOTDEF
double ag_scan_str(void *obj, char *name, char *defval)
{
    Agsym_t *attr;
    attr = agattr(obj, name);
    if (attr)
	return agxget(obj, attr->id);
    else
	return defval;
}
#endif
