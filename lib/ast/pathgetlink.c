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
* Glenn Fowler
* AT&T Bell Laboratories
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "errno.h"

#ifdef UNIV_MAX

#include <ctype.h>

#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#else
#include <compat_unistd.h>
#endif

/*
 * return external representation for symbolic link text of name in buf
 * the link text string length is returned
 */

int pathgetlink(const char *name, char *buf, int siz)
{
#ifdef WIN32
	return (-1);
#else
    int n;

    if ((n = readlink(name, buf, siz)) < 0)
	return (-1);
    if (n >= siz) {
	errno = EINVAL;
	return (-1);
    }
    buf[n] = 0;
#ifdef UNIV_MAX
    if (isspace(*buf)) {
	register char *s;
	register char *t;
	register char *u;
	register char *v;
	int match = 0;
	char tmp[PATH_MAX];

	s = buf;
	t = tmp;
	while (isalnum(*++s) || *s == '_' || *s == '.');
	if (*s++) {
	    for (;;) {
		if (!*s || isspace(*s)) {
		    if (match) {
			*t = 0;
			n = t - tmp;
			strcpy(buf, tmp);
		    }
		    break;
		}
		if (t >= &tmp[sizeof(tmp)])
		    break;
		*t++ = *s++;
		if (!match && t < &tmp[sizeof(tmp) - univ_size + 1])
		    for (n = 0; n < UNIV_MAX; n++) {
			if (*(v = s - 1) == *(u = univ_name[n])) {
			    while (*u && *v++ == *u)
				u++;
			    if (!*u) {
				match = 1;
				strcpy(t - 1, univ_cond);
				t += univ_size - 1;
				s = v;
				break;
			    }
			}
		    }
	    }
	}
    }
#endif
    return (n);
#endif
}
