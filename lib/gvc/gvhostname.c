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


/* This code is adapted from tclInixSock.c from Tcl sources. */

#include "config.h"

#ifdef HAVE_UNAME
#include <sys/utsname.h>
#endif
#include <netdb.h>

#include <string.h>

/*
 * There is no portable macro for the maximum length
 * of host names returned by gethostbyname().  We should only
 * trust SYS_NMLN if it is at least 255 + 1 bytes to comply with DNS
 * host name limits.
 *
 * Note:  SYS_NMLN is a restriction on "uname" not on gethostbyname!
 *
 * For example HP-UX 10.20 has SYS_NMLN == 9,  while gethostbyname()
 * can return a fully qualified name from DNS of up to 255 bytes.
 *
 * Fix suggested by Viktor Dukhovni (viktor@esm.com)
 */

#if defined(SYS_NMLN) && SYS_NMLEN >= 256
#define HOSTNAME_LEN SYS_NMLEN
#else
#define HOSTNAME_LEN 256
#endif

/*
 * The following variable holds the network name of this host.
 */

static char hostname[HOSTNAME_LEN + 1];
static int  hostnameInited = 0;
#if 0
TCL_DECLARE_MUTEX(hostMutex)
#endif

/*
 *----------------------------------------------------------------------
 *
 * gvhostname --
 *
 *      Returns the name of the local host.
 *
 * Results:
 *      A string containing the network name for this machine, or
 *      an empty string if we can't figure out the name.  The caller
 *      must not modify or free this string.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

char *
gvhostname(void)
{
#ifdef HAVE_UNAME
    struct utsname u;
    struct hostent *hp;
#else
    char buffer[sizeof(hostname)];
#endif
    char *native;

#if 0
    Tcl_MutexLock(&hostMutex);
#endif
    if (hostnameInited) {
#if 0
	Tcl_MutexUnlock(&hostMutex);
#endif
        return hostname;
    }

    native = NULL;
#ifdef HAVE_UNAME
    memset((void *) &u, (int) 0, sizeof(struct utsname));
    if (uname(&u) > -1) {                               /* INTL: Native. */
        hp = gethostbyname(u.nodename);                 /* INTL: Native. */
        if (hp == NULL) {
            /*
             * Sometimes the nodename is fully qualified, but gets truncated
             * as it exceeds SYS_NMLN.  See if we can just get the immediate
             * nodename and get a proper answer that way.
             */
            char *dot = strchr(u.nodename, '.');
            if (dot != NULL) {
                char *node = malloc((unsigned) (dot - u.nodename + 1));
                memcpy(node, u.nodename, (size_t) (dot - u.nodename));
                node[dot - u.nodename] = '\0';
                hp = gethostbyname(node);
                free(node);
            }
        }
        if (hp != NULL) {
            native = hp->h_name;
        } else {
            native = u.nodename;
        }
    }
#else
    /*
     * Uname doesn't exist; try gethostname instead.
     */

    if (gethostname(buffer, sizeof(buffer)) > -1) {     /* INTL: Native. */
        native = buffer;
    }
#endif

    if (native == NULL) {
        hostname[0] = 0;
    } else {
	strncpy(hostname, native, sizeof(hostname)-1);
        hostname[sizeof(hostname)-1] = 0;
    }
    hostnameInited = 1;
#if 0
    Tcl_MutexUnlock(&hostMutex);
#endif
    return hostname;
}

