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

/**
 * @param dval value to convert
 * @param n_digit number of digits wanted
 * @param decpt to return decimal point
 * @param sign to return sign
 */
char *sffcvt(double dval, int n_digit, int *decpt, int *sign)
{
    return _sfcvt(&dval, n_digit, decpt, sign, 0);
}
