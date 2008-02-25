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
 * return full path to p with mode access using $PATH
 * if a!=0 then it and $0 and $_ with $PWD are used for
 * related root searching
 * the related root must have a bin subdir
 * p==0 sets the cached relative dir to a
 * p==0 a=="" disables $0 $_ $PWD relative search
 * full path returned in path buffer
 * if path==0 then the space is malloc'd
 */

#include <ast.h>
#include <string.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#else
#include <compat_unistd.h>
#endif

/* #include <option.h> */
#ifdef WIN32
#define environ _environ
#else
extern char **environ;
#endif
char **opt_info_argv;

char *pathpath(register char *path, const char *p, const char *a, int mode)
{
    register char *s;
    char *x;
    char buf[PATH_MAX];

    static char *cmd;

    if (!path)
	path = buf;
    if (!p) {
	if (cmd)
	    free(cmd);
	cmd = a ? strdup(a) : (char *) 0;
	return 0;
    }
    if (strlen(p) < PATH_MAX) {
	strcpy(path, p);
	if (pathexists(path, mode))
	    return (path == buf) ? strdup(path) : path;
    }
    if (*p == '/')
	a = 0;
    else if ((s = (char *) a)) {
	x = s;
	if (strchr(p, '/')) {
	    a = p;
	    p = "..";
	} else
	    a = 0;
	if ((!cmd || *cmd) &&
	    (strchr(s, '/') ||
	     (((s = cmd) || (opt_info_argv && (s = *opt_info_argv))) &&
	      strchr(s, '/') && !strchr(s, '\n') && !access(s, F_OK)) ||
	     (environ && (s = *environ) && *s++ == '_' &&
	      *s++ == '=' && strchr(s, '/') && !strneq(s, "/bin/", 5) &&
	      !strneq(s, "/usr/bin/", 9)) ||
	     (*x && !access(x, F_OK) && (s = getenv("PWD")) && *s == '/')
	    )
	    ) {
	    if (!cmd)
		cmd = strdup(s);
	    if (strlen(s) < (sizeof(buf) - 6)) {
		s = strcopy(path, s);
		for (;;) {
		    do
			if (s <= path)
			    goto normal;
		    while (*--s == '/');
		    do
			if (s <= path)
			    goto normal;
		    while (*--s != '/');
		    strcpy(s + 1, "bin");
		    if (pathexists(path, PATH_EXECUTE)) {
			if ((s = pathaccess(path, path, p, a, mode)))
			    return path == buf ? strdup(s) : s;
			goto normal;
		    }
		}
	      normal:;
	    }
	}
    }
    x = !a && strchr(p, '/') ? "" : pathbin();
    if (!(s = pathaccess(path, x, p, a, mode)) && !*x
	&& (x = getenv("FPATH")))
	s = pathaccess(path, x, p, a, mode);
    return (s && path == buf) ? strdup(s) : s;
}
