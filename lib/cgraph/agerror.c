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

#include <stdio.h>
#include <cghdr.h>

agerrlevel_t agerrno;		/* Last error */
static agerrlevel_t agerrlevel = AGWARN;	/* Report errors >= agerrlevel */
static int agerrcnt;

static long aglast;		/* Last message */
static FILE *agerrout;		/* Message file */

void agseterr(agerrlevel_t lvl)
{
    agerrlevel = lvl;
}

char *aglasterr()
{
    long endpos;
    long len;
    char *buf;

    if (!agerrout)
	return 0;
    fflush(agerrout);
    endpos = ftell(agerrout);
    len = endpos - aglast;
    buf = (char*)malloc(len + 1);
    fseek(agerrout, aglast, SEEK_SET);
    fread(buf, sizeof(char), len, agerrout);
    buf[len] = '\0';
    fseek(agerrout, endpos, SEEK_SET);

    return buf;
}

static int agerr_va(agerrlevel_t level, char *fmt, va_list args)
{
    agerrlevel_t lvl;

    lvl = (level == AGPREV ? agerrno : (level == AGMAX) ? AGERR : level);

	agerrcnt++;
    agerrno = lvl;
    if (lvl >= agerrlevel) {
	if (level != AGPREV)
	    fprintf(stderr, "%s: ",
		    (level == AGERR) ? "Error" : "Warning");
	vfprintf(stderr, fmt, args);
	va_end(args);
	return 0;
    }

    if (!agerrout) {
	agerrout = tmpfile();
	if (!agerrout)
	    return 1;
    }

    if (level != AGPREV)
	aglast = ftell(agerrout);
    vfprintf(agerrout, fmt, args);
    va_end(args);
    return 0;
}

int agerr(agerrlevel_t level, char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    return agerr_va(level, fmt, args);
}

void agerrorf(char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    agerr_va(AGERR, fmt, args);
}

void agwarningf(char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    agerr_va(AGWARN, fmt, args);
}

int agerrors() { return agerrcnt; }
