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
 * AT&T Research
 *
 * include style search support
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <ast.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif
#include <string.h>

typedef struct Dir_s {		/* directory list element */
    struct Dir_s *next;		/* next in list                 */
    char dir[1];		/* directory path               */
} Dir_t;

static struct {			/* directory list state           */
    Dir_t *head;		/* directory list head          */
    Dir_t *tail;		/* directory list tail          */
} state;

/*
 * append dir to pathfind() include list
 */

int pathinclude(const char *dir)
{
    register Dir_t *dp;

    if (dir && *dir && !streq(dir, ".")) {
	if (!(dp = oldof(0, Dir_t, 1, strlen(dir))))
	    return -1;
	strcpy(dp->dir, dir);
	if (state.tail)
	    state.tail = state.tail->next = dp;
	else
	    state.head = state.tail = dp;
    }
    return 0;
}

/*
 * return path to name using pathinclude() list
 * path placed in <buf,size>
 * if lib!=0 then pathpath() attempted after include search
 * if type!=0 and name has no '.' then file.type also attempted
 * any *: prefix in lib is ignored (discipline library dictionary support)
 */

char *pathfind(const char *name, const char *lib, const char *type,
	       char *buf, size_t size)
{
    register Dir_t *dp;
    register char *s;
    char tmp[PATH_MAX];

    if (access(name, R_OK) >= 0)
	return strncpy(buf, name, size);
    if (type) {
	sfsprintf(buf, size, "%s.%s", name, type);
	if (access(buf, R_OK) >= 0)
	    return buf;
    }
    if (*name != '/') {
	if (strchr(name, '.'))
	    type = 0;
	for (dp = state.head; dp; dp = dp->next) {
	    sfsprintf(tmp, sizeof(tmp), "%s/%s", dp->dir, name);
	    if (pathpath(buf, tmp, "", PATH_REGULAR))
		return buf;
	    if (type) {
		sfsprintf(tmp, sizeof(tmp), "%s/%s.%s", dp->dir, name,
			  type);
		if (pathpath(buf, tmp, "", PATH_REGULAR))
		    return buf;
	    }
	}
	if (lib) {
	    if ((s = strrchr((char *) lib, ':')))
		lib = (const char *) s + 1;
	    sfsprintf(tmp, sizeof(tmp), "lib/%s/%s", lib, name);
	    if (pathpath(buf, tmp, "", PATH_REGULAR))
		return buf;
	    if (type) {
		sfsprintf(tmp, sizeof(tmp), "lib/%s/%s.%s", lib, name,
			  type);
		if (pathpath(buf, tmp, "", PATH_REGULAR))
		    return buf;
	    }
	}
    }
    return 0;
}
