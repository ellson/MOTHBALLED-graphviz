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
 *  This library forms the socket for run-time loadable device plugins.  
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

/* experimenting with in-memory deflation so as to write compressed files to channels and strings */
//  #define IN_MEM_COMPRESSION

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

#ifdef IN_MEM_COMPRESSION
#ifndef OS_CODE
#  define OS_CODE  0x03  /* assume Unix */
#endif
static char z_file_header[] =
   {0x1f, 0x8b, /*magic*/ Z_DEFLATED, 0 /*flags*/, 0,0,0,0 /*time*/, 0 /*xflags*/, OS_CODE};

static z_stream z_strm;
static char *df;
static unsigned int dfallocated;
static unsigned long int crc;
#endif
#endif

#include "const.h"
#include "gvplugin_device.h"
#include "gvcjob.h"
#include "gvcint.h"
#include "gvcproc.h"
#include "logic.h"

#ifdef WITH_CODEGENS
extern FILE* Output_file;
#endif

static const int PAGE_ALIGN = 4095;		/* align to a 4K boundary (less one), typical for Linux, Mac OS X and Windows memory allocation */

static size_t gvdevice_write_no_z (GVJ_t * job, const char *s, unsigned int len)
{
    if (job->gvc->write_fn)   /* externally provided write dicipline */
	return (job->gvc->write_fn)(job, (char*)s, len);
    if (job->output_data) {
	if (len > job->output_data_allocated - (job->output_data_position + 1)) {
	    /* ensure enough allocation for string = null terminator */
	    job->output_data_allocated = (job->output_data_position + len + 1 + PAGE_ALIGN) & ~PAGE_ALIGN;
	    job->output_data = realloc(job->output_data, job->output_data_allocated);
	    if (!job->output_data) {
                (job->common->errorfn) ("memory allocation failure\n");
		exit(1);
	    }
	}
	memcpy(job->output_data + job->output_data_position, s, len);
        job->output_data_position += len;
	job->output_data[job->output_data_position] = '\0'; /* keep null termnated */
	return len;
    }
    else
	return fwrite(s, sizeof(char), len, job->output_file);
    return 0;
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

#ifndef IN_MEM_COMPRESSION
        if (job->flags & GVDEVICE_COMPRESSED_FORMAT) {
#if HAVE_LIBZ
	    int fd;

	    /* open dup so can gzclose independent of FILE close */
	    fd = dup(fileno(job->output_file));
	    job->output_file = (FILE *) (gzdopen(fd, "wb"));
	    if (!job->output_file) {
		(job->common->errorfn) ("Error initializing deflation on output file\n");
		exit(1);
	    }
#else
	    (job->common->errorfn) ("No libz support.\n");
	    exit(1);
#endif
        }
#endif
    }

#ifdef IN_MEM_COMPRESSION
    if (job->flags & GVDEVICE_COMPRESSED_FORMAT) {
#ifdef HAVE_LIBZ
	z_stream *z = &z_strm;

	z->zalloc = (alloc_func)0;
	z->zfree = (free_func)0;
	z->opaque = (voidpf)0;
	z->next_in = NULL;
	z->next_out = NULL;
	z->avail_in = 0;

	crc = crc32(0L, Z_NULL, 0);

	if (deflateInit2(z, Z_DEFAULT_COMPRESSION, Z_DEFLATED, -MAX_WBITS, MAX_MEM_LEVEL, Z_DEFAULT_STRATEGY) != Z_OK) {
	    (job->common->errorfn) ("Error initializing for deflation\n");
	    exit(1);
	}
	gvdevice_write_no_z(job, z_file_header, sizeof(z_file_header));
#else
	(job->common->errorfn) ("No libz support.\n");
	exit(1);
#endif
    }
#endif
}

size_t gvdevice_write (GVJ_t * job, const char *s, unsigned int len)
{
    if (!len || !s)
	return 0;

#ifdef IN_MEM_COMPRESSION
    if (job->flags & GVDEVICE_COMPRESSED_FORMAT) {
#ifdef HAVE_LIBZ
	z_streamp z = &z_strm;
	int ret, dflen;

	dflen = deflateBound(z, len);
	if (dfallocated < dflen) {
	    dfallocated = (dflen + 1 + PAGE_ALIGN) & ~PAGE_ALIGN;
	    df = realloc(df, dfallocated);
	    if (! df) {
                (job->common->errorfn) ("memory allocation failure\n");
		exit(1);
	    }
	}

	z->next_in = s;
	z->avail_in = len;
	z->next_out = df;
	z->avail_out = dfallocated;
	ret=deflate (z, Z_NO_FLUSH);
	if (ret != Z_OK) {
            (job->common->errorfn) ("deflation problem %d\n", ret);
	    exit(1);
	}
	crc = crc32(crc, s, len);
	len = z->next_out - df;
	s = df;
#endif
    }
#else
    if (!(job->gvc->write_fn) && (job->flags & GVDEVICE_COMPRESSED_FORMAT)) {
#ifdef HAVE_LIBZ
	return gzwrite((gzFile *) (job->output_file), s, len);
#endif
    }
#endif
    return gvdevice_write_no_z (job, s, len);
}

void gvdevice_fputs(GVJ_t * job, const char *s)
{
    gvdevice_write (job, s, strlen(s));
}

static void gvdevice_flush(GVJ_t * job)
{
    if (job->output_file
      && ! job->external_context
      && ! job->gvc->write_fn
      && job->output_lang != TK
      && ! (job->flags & GVDEVICE_COMPRESSED_FORMAT)) {
	fflush(job->output_file);
    }
}

static void gvdevice_close(GVJ_t * job)
{
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

void gvdevice_format(GVJ_t * job)
{
    gvdevice_engine_t *gvde = job->device.engine;

    if (gvde && gvde->format)
	gvde->format(job);
    gvdevice_flush(job);
}

void gvdevice_finalize(GVJ_t * job)
{
    gvdevice_engine_t *gvde = job->device.engine;
    boolean finalized_p = FALSE;

#ifdef IN_MEM_COMPRESSION
    if (job->flags & GVDEVICE_COMPRESSED_FORMAT) {
#ifdef HAVE_LIBZ
	z_streamp z = &z_strm;
	char out[8] = "";
	int ret;
	int cnt = 0;

	z->next_in = out;
	z->avail_in = 0;
	z->next_out = df;
	z->avail_out = dfallocated;

#if 1
	while ((ret = deflate (z, Z_FINISH)) == Z_OK && (cnt++ <= 100)) {
	    gvdevice_write_no_z(job, df, z->next_out - df);
	    z->next_in = out;
	    z->avail_in = 0;
	    z->next_out = df;
	    z->avail_out = dfallocated;
	}
#else
	ret = deflate (z, Z_FINISH);
#endif
	if (ret != Z_STREAM_END && ret != Z_OK) {
            (job->common->errorfn) ("deflation finish problem %d cnt=%d\n", ret, cnt);
	    exit(1);
	}
	gvdevice_write_no_z(job, df, z->next_out - df);

	ret = deflateEnd(z);
	if (ret != Z_OK) {
	    (job->common->errorfn) ("deflation end problem %d\n", ret);
	    exit(1);
	}
	out[0] = crc;
	out[1] = crc >> 8;
	out[2] = crc >> 16;
	out[3] = crc >> 24;
	out[4] = z->total_in;
	out[5] = z->total_in >> 8;
	out[6] = z->total_in >> 16;
	out[7] = z->total_in >> 24;
	gvdevice_write_no_z(job, out, sizeof(out));
#else
	(job->common->errorfn) ("No libz support\n");
	exit(1);
#endif
    }
#endif

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
#ifndef IN_MEM_COMPRESSION
        if (job->flags & GVDEVICE_COMPRESSED_FORMAT) {
#ifdef HAVE_LIBZ
	    gzclose((gzFile *) (job->output_file));
	    job->output_file = NULL;
#else
	    (job->common->errorfn) ("No libz support\n");
	    exit(1);
#endif
	}
#endif
	gvdevice_flush(job);
	gvdevice_close(job);
    }
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
    char buf[BUFSIZ];
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
#define val_str(n, x) static double n = x; static char n##str[] = #x;
val_str(maxnegnum, -999999999999999.99)

/* we use len and don't need the string to be terminated */
/* #define TERMINATED_NUMBER_STRING */

/* Note.  Returned string is only good until the next call to gvprintnum */
static char * gvprintnum (int *len, double number)
{
    static char tmpbuf[sizeof(maxnegnumstr)];   /* buffer big enough for worst case */
    char *result = tmpbuf+sizeof(maxnegnumstr); /* init result to end of tmpbuf */
    long int N;
    boolean showzeros, negative;
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
	return "0";
    }
    if ((negative = (N < 0)))		/* avoid "-0" by testing rounded int */
        N = -N;				/* make number +ve */
#ifdef TERMINATED_NUMBER_STRING
    *--result = '\0';			/* terminate the result string */
#endif
    showzeros = FALSE;			/* don't print trailing zeros */
    for (i = DECPLACES; N || i > 0; i--) {  /* non zero remainder,
						or still in fractional part */
        digit = N % 10;			/* next least-significant digit */
        N /= 10;
        if (digit || showzeros) {	/* if digit is non-zero,
						or if we are printing zeros */
            *--result = digit | '0';	/* convert digit to ascii */
            showzeros = TRUE;		/* from now on we must print zeros */
        }
        if (i == 1) {			/* if completed fractional part */
            if (showzeros)		/* if there was a non-zero fraction */
                *--result = '.';	/* print decimal point */
            showzeros = TRUE;		/* print all digits in int part */
        }
    }
    if (negative)			/* print "-" if needed */
        *--result = '-';
#ifdef TERMINATED_NUMBER_STRING
    *len = tmpbuf+sizeof(maxnegnumstr)-1 - result;
#else
    *len = tmpbuf+sizeof(maxnegnumstr) - result;
#endif
    return result;				
}


#ifdef GVPRINTNUM_TEST
int main (int argc, char *argv[])
{
    char *buf;
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
    char *buf;
    int len;

    buf = gvprintnum(&len, num);
    gvdevice_write(job, buf, len);
} 

void gvdevice_printpointf(GVJ_t * job, pointf p)
{
    char *buf;
    int len;

    buf = gvprintnum(&len, p.x);
    gvdevice_write(job, buf, len);
    gvdevice_write(job, " ", 1);
    buf = gvprintnum(&len, p.y);
    gvdevice_write(job, buf, len);
} 

void gvdevice_printpointflist(GVJ_t * job, pointf *p, int n)
{
    int i = 0;

    while (TRUE) {
	gvdevice_printpointf(job, p[i]);
        if (++i >= n) break;
        gvdevice_write(job, " ", 1);
    }
} 

