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
 *  graphics code generator wrapper
 *
 *  This library forms the socket for run-time loadable device plugins.  
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef WIN32
#include <fcntl.h>
#include <io.h>
#include "compat.h"
#endif

#ifdef HAVE_LIBZ
#include <zlib.h>
#endif

#include "const.h"
#include "gvplugin_device.h"
#include "gvcjob.h"
#include "gvcint.h"
#include "gvcproc.h"

size_t gvdevice_write (GVJ_t * job, const unsigned char *s, unsigned int len)
{
    if (job->gvc->write_fn && job->output_file == stdout)   /* externally provided write dicipline */
	return (job->gvc->write_fn)((char*)s, len);
    if (job->flags & GVDEVICE_COMPRESSED_FORMAT) {
#ifdef HAVE_LIBZ
	return gzwrite((gzFile *) (job->output_file), s, len);
#endif
    }
    else if (job->output_data) {
	if (len > (job->output_data_allocated - (job->output_data_position + 1))) {
	    job->output_data_allocated = job->output_data_position + len + 1000;
	    job->output_data = realloc(job->output_data, job->output_data_allocated);
	    if (!job->output_data) {
		fprintf(stderr, "failure realloc'ing for result string\n");
		return 0;
	    }
	}
	strcpy(job->output_data + job->output_data_position, (char*)s);
        job->output_data_position += len;
	return len;
    }
    else
	return fwrite(s, sizeof(char), len, job->output_file);
    return 0;
}

void gvdevice_fputs(GVJ_t * job, char *s)
{
    gvdevice_write (job, (unsigned char*)s, strlen(s));
}

/* gvdevice_printf:
 * Note that this function is unsafe due to the fixed buffer size.
 * It should only be used when the caller is sure the input will not
 * overflow the buffer. In particular, it should be avoided for
 * input coming from users. Also, if vsnprintf is available, the
 * code should check for return values to use it safely.
 */
void gvdevice_printf(GVJ_t * job, const char *format, ...)
{
    unsigned char buf[BUFSIZ];
    unsigned int len;
    va_list argp;

    va_start(argp, format);
#ifdef HAVE_VSNPRINTF
    len = vsnprintf((char *)buf, sizeof(buf), format, argp);
#else
    len = vsprintf((char *)buf, format, argp);
#endif
    va_end(argp);

    gvdevice_write(job, buf, len);
}


/* Test with:
 *	cc -DGVPRINTNUM_TEST gvprintnum.c -o gvprintnum
 */

#define DECPLACES 2
#define DECPLACES_SCALE 100

/* use macro so maxnegnum is stated just once for both double and string versions */
#define val_str(n, x) static double n = x; static unsigned char n##str[] = #x;
val_str(maxnegnum, -999999999999999.99)

/* Note.  Returned string is only good until the next call to gvprintnum */
static unsigned char * gvprintnum (int *len, double number)
{
    static unsigned char tmpbuf[sizeof(maxnegnumstr)];   /* buffer big enough for worst case */
    unsigned char *result = tmpbuf+sizeof(maxnegnumstr); /* init result to end of tmpbuf */
    long int N;
    bool showzeros, negative;
    int digit, i;

    /*
        number limited to a working range: maxnegnum >= n >= -maxnegnum
	N = number * DECPLACES_SCALE rounded towards zero,
	printing to buffer in reverse direction,
	printing "." after DECPLACES
	suppressing trailing "0" and "."
     */

    if (number < maxnegnum) {		/* -ve limit */
	*len = sizeof(maxnegnumstr)-1;  /* len doesn't include terminator */
	return maxnegnumstr;;
    }
    if (number > -maxnegnum) {		/* +ve limit */
	*len = sizeof(maxnegnumstr)-2;  /* len doesn't include terminator or sign */
	return maxnegnumstr+1;		/* +1 to skip the '-' sign */
    }
    number *= DECPLACES_SCALE;		/* scale by DECPLACES_SCALE */
    if (number < 0.0)			/* round towards zero */
        N = number - 0.5;
    else
        N = number + 0.5;
    if (N == 0) {			/* special case for exactly 0 */
	*len = 1;
	return (unsigned char *)"0";
    }
    if ((negative = (N < 0)))		/* avoid "-0" by testing rounded int */
        N = -N;				/* make number +ve */
    *--result = '\0';			/* terminate the result string */
    showzeros = false;			/* don't print trailing zeros */
    for (i = DECPLACES; N || i > 0; i--) {  /* non zero remainder,
						or still in fractional part */
        digit = N % 10;			/* next least-significant digit */
        N /= 10;
        if (digit || showzeros) {	/* if digit is non-zero,
						or if we are printing zeros */
            *--result = digit | '0';	/* convert digit to ascii */
            showzeros = true;		/* from now on we must print zeros */
        }
        if (i == 1) {			/* if completed fractional part */
            if (showzeros)		/* if there was a non-zero fraction */
                *--result = '.';	/* print decimal point */
            showzeros = true;		/* print all digits in int part */
        }
    }
    if (negative)			/* print "-" if needed */
        *--result = '-';
    *len = tmpbuf+sizeof(maxnegnumstr)-1 - result;
    return result;				
}


#ifdef GVPRINTNUM_TEST
int main (int argc, char *argv[])
{
    unsigned char *buf;
    int len;

    double test[] = {
	-maxnegnum*1.1, -maxnegnum*.9,
	1e8, 10.008, 10, 1, .1, .01,
	.006, .005, .004, .001, 1e-8, 
	0, -0,
	-1e-8, -.001, -.004, -.005, -.006,
	-.01, -.1, -1, -10, -10.008, -1e8,
	maxnegnum*.9, maxnegnum*1.1
    };
    int i = sizeof(test) / sizeof(test[0]);

    while (i--) {
	buf = gvprintnum(&len, test[i]);
        fprintf (stdout, "%g = %s %d\n", test[i], buf, len);
    }

    return 0;
}
#endif

void gvdevice_printnum(GVJ_t * job, double num)
{
    unsigned char *buf;
    int len;

    buf = gvprintnum(&len, num);
    gvdevice_write(job, buf, len);
} 

void gvdevice_printpointf(GVJ_t * job, pointf p)
{
    unsigned char *buf;
    int len;

    buf = gvprintnum(&len, p.x);
    gvdevice_write(job, buf, len);
    gvdevice_write(job, (unsigned char*)" ", 1);
    buf = gvprintnum(&len, p.y);
    gvdevice_write(job, buf, len);
} 

void gvdevice_printpointflist(GVJ_t * job, pointf *p, int n)
{
    int i = 0;

    while (TRUE) {
	gvdevice_printpointf(job, p[i]);
        if (++i >= n) break;
        gvdevice_write(job, (unsigned char*)" ", 1);
    }
} 

static void auto_output_filename(GVJ_t *job)
{
    static char *buf;
    static int bufsz;
    char gidx[100];  /* large enough for '.' plus any integer */
    char *fn, *p;
    int len;

    if (job->graph_index)
        sprintf(gidx, ".%d", job->graph_index + 1);
    else
        gidx[0] = '\0';
    if (!(fn = job->input_filename))
        fn = "noname.dot";
    len = strlen(fn)                    /* typically "something.dot" */
        + strlen(gidx)                  /* "", ".2", ".3", ".4", ... */
        + 1                             /* "." */
        + strlen(job->output_langname)  /* e.g. "png" */
        + 1;                            /* null terminaor */
    if (bufsz < len) {
            bufsz = len + 10;
            buf = realloc(buf, bufsz * sizeof(char));
    }
    strcpy(buf, fn);
    strcat(buf, gidx);
    strcat(buf, ".");
    if ((p = strchr(job->output_langname, ':'))) {
        strcat(buf, p+1);
        strcat(buf, ".");
        strncat(buf, job->output_langname, (p - job->output_langname));
    }
    else {
        strcat(buf, job->output_langname);
    }

    job->output_filename = buf;
}

#ifdef WITH_CODEGENS
extern FILE* Output_file;
#endif

void gvdevice_initialize(GVJ_t * job)
{
    gvdevice_engine_t *gvde = job->device.engine;
    GVC_t *gvc = job->gvc;

    if (gvde && gvde->initialize) {
	gvde->initialize(job);
    }
    else if (job->output_data) {
    }
    /* if the device has no initialization then it uses file output */
    else if (!job->output_file) {        /* if not yet opened */
        if (gvc->common.auto_outfile_names)
            auto_output_filename(job);
        if (job->output_filename) {
            job->output_file = fopen(job->output_filename, "w");
            if (job->output_file == NULL) {
                perror(job->output_filename);
                exit(1);
            }
        }
        else
            job->output_file = stdout;

#ifdef WITH_CODEGENS
        Output_file = job->output_file;
#endif

#ifdef HAVE_SETMODE
#ifdef O_BINARY
        if (job->flags & GVDEVICE_BINARY_FORMAT)
            setmode(fileno(job->output_file), O_BINARY);
#endif
#endif

        if (job->flags & GVDEVICE_COMPRESSED_FORMAT) {
#if HAVE_LIBZ
	    int fd;

	    /* open dup so can gzclose independent of FILE close */
	    fd = dup(fileno(job->output_file));
	    job->output_file = (FILE *) (gzdopen(fd, "wb"));
	    if (!job->output_file) {
		(job->common->errorfn) ("Error initializing compression on output file\n");
		exit(1);
	    }
#else
	    (job->common->errorfn) ("No libz support.\n");
	    exit(1);
#endif
        }
    }
}

void gvdevice_format(GVJ_t * job)
{
    gvdevice_engine_t *gvde = job->device.engine;

    if (gvde && gvde->format)
	gvde->format(job);
    if (job->output_file
      && ! job->external_context
      && job->output_lang != TK
      && ! (job->flags & GVDEVICE_COMPRESSED_FORMAT))
	fflush(job->output_file);
}

void gvdevice_finalize(GVJ_t * job)
{
    gvdevice_engine_t *gvde = job->device.engine;
    boolean finalized_p = FALSE;

    if (gvde) {
	if (gvde->finalize) {
	    gvde->finalize(job);
	    finalized_p = TRUE;
	}
    }
#ifdef WITH_CODEGENS
    else {
	codegen_t *cg = job->codegen;

	if (cg && cg->reset)
	    cg->reset();
    }
#endif

    if (! finalized_p) {
        /* if the device has no finalization then it uses file output */
	if (job->flags & GVDEVICE_COMPRESSED_FORMAT) {
#ifdef HAVE_LIBZ
	    gzclose((gzFile *) (job->output_file));
	    job->output_file = NULL;
#else
	    (job->common->errorfn) ("No libz support\n");
	    exit(1);
#endif
	}
	if (job->output_filename
	  && job->output_file != stdout 
	  && ! job->external_context) {
	    if (job->output_file) {
	        fclose(job->output_file);
	        job->output_file = NULL;
	    }
            job->output_filename = NULL;
	}
    }
}
