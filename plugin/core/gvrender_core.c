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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#ifdef HAVE_LIBZ
#include <zlib.h>
#endif

#ifdef WIN32
#include <io.h>
#include "compat.h"
#endif

#include "gvplugin_render.h"

void core_init_compression(GVJ_t *job, compression_t compression)
{
#if HAVE_LIBZ
    int fd;
#endif

    switch ((job->compression = compression)) {
    case COMPRESSION_ZLIB:
#if HAVE_LIBZ
        /* open dup so can gzclose independent of FILE close */
        fd = dup(fileno(job->output_file));
#ifdef HAVE_SETMODE
#ifdef O_BINARY
        /*
	 * Windows will do \n -> \r\n  translations on
	 * stdout unless told otherwise.
	 */
        setmode(fd, O_BINARY);
#endif
#endif

        job->output_file = (FILE *) (gzdopen(fd, "wb"));
        if (!job->output_file) {
            (job->common->errorfn) ("Error initializing compression on output file\n");
            exit(1);
        }
        break;
#else
        (job->common->errorfn) ("No libz support.\n");
        exit(1);
#endif
    case COMPRESSION_NONE:
        break;
    }
}

void core_fini_compression(GVJ_t *job)
{
    switch (job->compression) {
    case COMPRESSION_ZLIB:
#ifdef HAVE_LIBZ
        gzclose((gzFile *) (job->output_file));
	job->output_file = NULL;
        break;
#else
        (job->common->errorfn) ("No libz support\n");
        exit(1);
#endif
    case COMPRESSION_NONE:
        break;
    }
}

void core_fputs(GVJ_t * job, char *s)
{
    int len, rc;

    len = strlen(s);
    switch (job->compression) {
    case COMPRESSION_ZLIB:
#ifdef HAVE_LIBZ
	gzwrite((gzFile *) (job->output_file), s, (unsigned) len);
#endif
	break;
    case COMPRESSION_NONE:
	rc = fwrite(s, sizeof(char), (unsigned) len, job->output_file);
	break;
    }
}

/* core_printf:
 * Note that this function is unsafe due to the fixed buffer size.
 * It should only be used when the caller is sure the input will not
 * overflow the buffer. In particular, it should be avoided for
 * input coming from users. Also, if vsnprintf is available, the
 * code should check for return values to use it safely.
 */
void core_printf(GVJ_t * job, const char *format, ...)
{
    char buf[BUFSIZ];
    va_list argp;

    va_start(argp, format);
#ifdef HAVE_VSNPRINTF
    (void) vsnprintf(buf, sizeof(buf), format, argp);
#else
    (void) vsprintf(buf, format, argp);
#endif
    va_end(argp);

    core_fputs(job, buf);
}
