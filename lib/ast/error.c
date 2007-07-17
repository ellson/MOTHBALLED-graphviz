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
 * standalone mini error implementation
 */

#ifdef WIN32
#include <config.h>

#ifdef GVDLL
#define _BLD_sfio 1
#endif
#endif

#include <ast.h>
#include <error.h>
#include <string.h>
#include <errno.h>

Error_info_t error_info;

void setErrorLine (int line) { error_info.line = line; }
void setErrorFileLine (char* src, int line) {
    error_info.file = src;
    error_info.line = line;
}
void setErrorId (char* id) { error_info.id = id; }
void setErrorErrors (int errors) { error_info.errors = errors; }
int  getErrorErrors () { return error_info.errors; }

void errorv(const char *id, int level, va_list ap)
{
    char *s;
    int flags;

    if (level < 0)
	flags = 0;
    else {
	flags = level & ~ERROR_LEVEL;
	level &= ERROR_LEVEL;
    }
    if (level && ((s = error_info.id) || (s = (char *) id))) {
	if (flags & ERROR_USAGE)
	    sfprintf(sfstderr, "Usage: %s ", s);
	else
	    sfprintf(sfstderr, "%s: ", s);
    }
    if (flags & ERROR_USAGE)
	/*nop */ ;
    else if (level < 0) {
	int i;
	for (i = 0; i < error_info.indent; i++)
	    sfprintf(sfstderr, "  ");
	sfprintf(sfstderr, "debug%d: ", level);
    } else if (level) {
	if (level == ERROR_WARNING) {
	    sfprintf(sfstderr, "warning: ");
	    error_info.warnings++;
	} else {
	    error_info.errors++;
	    if (level == ERROR_PANIC)
		sfprintf(sfstderr, "panic: ");
	}
	if (error_info.line) {
	    if (error_info.file && *error_info.file)
		sfprintf(sfstderr, "\"%s\", ", error_info.file);
	    sfprintf(sfstderr, "line %d: ", error_info.line);
	}
    }
    s = va_arg(ap, char *);
    sfvprintf(sfstderr, s, ap);
    if (flags & ERROR_SYSTEM)
	sfprintf(sfstderr, "\n%s", strerror(errno));
    sfprintf(sfstderr, "\n");
    if (level >= ERROR_FATAL)
	exit(level - ERROR_FATAL + 1);
}

void error(int level, ...)
{
    va_list ap;

    va_start(ap, level);
    errorv(NiL, level, ap);
    va_end(ap);
}

void errorf(void *handle, void *discipline, int level, ...)
{
    va_list ap;

    va_start(ap, level);
    errorv((discipline
	    && handle) ? *((char **) handle) : (char *) handle, level, ap);
    va_end(ap);
}
