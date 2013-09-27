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

#include "inkpot_xlate.h"

void hsva2rgba(double hsva[4], double rgba[4])
{
    int i;
    double f, p, q, t, h, s, v;

    rgba[3] = hsva[3]; /* copy alpha */

    h = (hsva[0] < 0.0) ? 0.0 : hsva[0];
    h = (h > 1.0) ? 1.0 : h;
    s = (hsva[1] < 0.0) ? 0.0 : hsva[1];
    s = (s > 1.0) ? 1.0 : s;
    v = (hsva[2] < 0.0) ? 0.0 : hsva[2];
    v = (v > 1.0) ? 1.0 : v;

    if (s <= 0.0) {		/* achromatic */
	rgba[0] = v;
	rgba[1] = v;
	rgba[2] = v;
    } else {
	if (h >= 1.0)
	    h = 0.0;
	h = 6.0 * h;
	i = (int) h;
	f = h - (double) i;
	p = v * (1 - s);
	q = v * (1 - (s * f));
	t = v * (1 - (s * (1 - f)));
	switch (i) {
	case 0:
	    rgba[0] = v;
	    rgba[1] = t;
	    rgba[2] = p;
	    break;
	case 1:
	    rgba[0] = q;
	    rgba[1] = v;
	    rgba[2] = p;
	    break;
	case 2:
	    rgba[0] = p;
	    rgba[1] = v;
	    rgba[2] = t;
	    break;
	case 3:
	    rgba[0] = p;
	    rgba[1] = q;
	    rgba[2] = v;
	    break;
	case 4:
	    rgba[0] = t;
	    rgba[1] = p;
	    rgba[2] = v;
	    break;
	case 5:
	    rgba[0] = v;
	    rgba[1] = p;
	    rgba[2] = q;
	    break;
	}
    }
}

void rgba2hsva(double rgba[4], double hsva[4])
{

    double rgbmin, rgbmax;
    double rc, bc, gc;
    double ht = 0.0, st = 0.0;

    hsva[3] = rgba[3];  /* copy alpha */

    rgbmin = (rgba[1] < rgba[2]) ? rgba[1] : rgba[2];
    rgbmin = (rgba[0] < rgbmin ) ? rgba[0] : rgbmin;

    rgbmax = (rgba[1] > rgba[2]) ? rgba[1] : rgba[2];
    rgbmax = (rgba[0] > rgbmax ) ? rgba[0] : rgbmax;

    if (rgbmax > 0.0)
	st = (rgbmax - rgbmin) / rgbmax;

    if (st > 0.0) {
	rc = (rgbmax - rgba[0]) / (rgbmax - rgbmin);
	gc = (rgbmax - rgba[1]) / (rgbmax - rgbmin);
	bc = (rgbmax - rgba[2]) / (rgbmax - rgbmin);
	if (rgba[0] == rgbmax)
	    ht = bc - gc;
	else if (rgba[1] == rgbmax)
	    ht = 2 + rc - bc;
	else if (rgba[2] == rgbmax)
	    ht = 4 + gc - rc;
	ht = ht * 60.0;
	if (ht < 0.0)
	    ht += 360.0;
    }
    hsva[0] = ht / 360.0;
    hsva[1] = st;
    hsva[2] = rgbmax;
}

void rgba2cmyk(double rgba[4], double cmyk[4])
{
    double c, m, y, k;

    /* alpha is ignored */

    c = 1.0 - rgba[0];
    m = 1.0 - rgba[1];
    y = 1.0 - rgba[2];
    k = c < m ? c : m;
    k = y < k ? y : k;
    cmyk[4] = k;
    cmyk[3] = c - k;
    cmyk[2] = m - k;
    cmyk[0] = y - k;
}
