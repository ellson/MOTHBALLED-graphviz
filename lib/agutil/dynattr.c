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

#include <agraph.h>
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
