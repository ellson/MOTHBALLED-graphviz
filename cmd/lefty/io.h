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

#ifdef __cplusplus
extern "C" {
#endif


/* Lefteris Koutsofios - AT&T Bell Laboratories */

#ifndef _IO_H
#define _IO_H

    typedef enum {
	IO_FILE, IO_PTY, IO_PIPE, IO_SOCKET,
#ifdef FEATURE_CS
	IO_CS,
#endif
	IO_SIZE
    } iotype_t;

    typedef struct io_t {
	int inuse, ismonitored;
	iotype_t type;
	FILE *ifp, *ofp;
	int pid;
	char *buf;
    } io_t;

#ifdef FEATURE_MS
#define IOmonitor(ioi, set) do { \
    iop[ioi].ismonitored = TRUE; \
} while (0)
#define IOunmonitor(ioi, set) do { \
    iop[ioi].ismonitored = FALSE; \
} while (0)
#else
#define IOmonitor(ioi, set) do { \
    iop[ioi].ismonitored = TRUE; \
    FD_SET (fileno (iop[ioi].ifp), &set); \
} while (0)
#define IOunmonitor(ioi, set) do { \
    iop[ioi].ismonitored = FALSE; \
    FD_CLR (fileno (iop[ioi].ifp), &set); \
} while (0)
#endif

#define IOismonitored(ioi) (iop[ioi].ismonitored == TRUE)
#define IOINCR 5
#define IOSIZE sizeof (io_t)
#define IOBUFSIZE 2048

    extern io_t *iop;
    extern int ion;

    void IOinit(void);
    void IOterm(void);
    int IOopen(char *, char *, char *, char *);
    int IOclose(int, char *);
    int IOreadline(int, char *, int);
    int IOread(int, char *, int);
    int IOwriteline(int, char *);

#endif				/* _IO_H */

#ifdef __cplusplus
}
#endif
