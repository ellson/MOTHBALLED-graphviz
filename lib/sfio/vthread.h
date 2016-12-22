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

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _VTHREAD_H
#define _VTHREAD_H	1

#define VTHREAD_VERSION    20001201L

/*	Header for the Vthread library.
**	Note that the macro vt_threaded may be defined
**	outside of vthread.h to suppress threading.
**
**	Written by Kiem-Phong Vo
*/

#include	<errno.h>

#include "config.h"

#ifdef HAVE_SYS_TYPES_H
#   include <sys/types.h>
#endif // HAVE_SYS_TYPES_H

#undef vt_threaded

#ifndef vt_threaded
#define vt_threaded		0
#endif

/* common attributes for various structures */
#define VT_RUNNING	000000001	/* thread is running            */
#define VT_SUSPENDED	000000002	/* thread is suspended          */
#define VT_WAITED	000000004	/* thread has been waited       */
#define VT_FREE		000010000	/* object can be freed          */
#define VT_INIT		000020000	/* object was initialized       */
#define VT_BITS		000030007	/* bits that we care about      */

/* directives for vtset() */
#define VT_STACK	1	/* set stack size               */

    typedef struct _vtmutex_s Vtmutex_t;
    typedef struct _vtonce_s Vtonce_t;
    typedef struct _vthread_s Vthread_t;

#ifndef EINVAL
#define EINVAL			22
#endif
#ifndef EBUSY
#define EBUSY			16
#endif
#ifndef EDEADLK
#define EDEADLK			45
#endif
#ifndef EPERM
#define EPERM			1
#endif

    extern Vthread_t *vtopen(Vthread_t *, int);
    extern int vtclose(Vthread_t *);
    extern int vtset(Vthread_t *, int, void *);
    extern int vtrun(Vthread_t *, void *(*)(void *), void *);
    extern int vtkill(Vthread_t *);
    extern int vtwait(Vthread_t *);

/*    extern int vtonce(Vtonce_t *, void (*)()); */

    extern Vtmutex_t *vtmtxopen(Vtmutex_t *, int);
    extern int vtmtxclose(Vtmutex_t *);
    extern int vtmtxlock(Vtmutex_t *);
    extern int vtmtxtrylock(Vtmutex_t *);
    extern int vtmtxunlock(Vtmutex_t *);
    extern int vtmtxclrlock(Vtmutex_t *);

    extern void *vtstatus(Vthread_t *);
    extern int vterror(Vthread_t *);
    extern int vtmtxerror(Vtmutex_t *);
    extern int vtonceerror(Vtonce_t *);

#if defined(vt_threaded) && vt_threaded
/* mutex structure */
	struct _vtmutex_s {
	_vtmtx_t lock;
	int count;
	_vtid_t owner;
	int state;
	int error;
    };

/* structure for states of thread */
    struct _vthread_s {
	_vtself_t self;		/* self-handle          */
	_vtid_t id;		/* thread id            */
	_vtattr_t attrs;	/* attributes           */
	size_t stack;		/* stack size           */
	int state;		/* execution state      */
	int error;		/* error status         */
	void *exit;		/* exit value           */
    };

/* structure for exactly once execution */
    struct _vtonce_s {
	int done;
	_vtonce_t once;
	int error;
    };

#if defined(_WIN32)
#define VTONCE_INITDATA		{0, 0}
#else
#define VTONCE_INITDATA		{0, PTHREAD_ONCE_INIT }
#endif

#define vtstatus(vt)		((vt)->exit)
#define vterror(vt)		((vt)->error)
#define vtmtxerror(mtx)		((mtx)->error)
#define vtonceerror(once)	((once)->error)

#endif				/*vt_threaded */

/* fake structures and functions */
#if defined(vt_threaded) && !vt_threaded
    struct _vtmutex_s {
	int error;
    };
    struct _vtattr_s {
	int error;
    };
    struct _vthread_s {
	int error;
    };
    struct _vtonce_s {
	int error;
    };

#define VTONCE_INITDATA		{0}

#define vtopen(vt,flgs)		((Vthread_t*)0)
#define vtclose(vt)		(-1)
#define vtkill(vt)		(-1)
#define vtwait(vt)		(-1)
#define vtrun(vt,fn,arg)	(-1)

#define vtset(vt,t,v)		(-1)
#define vtonce(on,fu)		(-1)

#define vtmtxopen(mtx,flgs)	(void)((Vtmutex_t*)0)
#define vtmtxclose(mtx)		(void)(-1)
#define vtmtxlock(mtx)		(void)(-1)
#define vtmtxtrylock(mtx)	(-1)
#define vtmtxunlock(mtx)	(void)(-1)
#define vtmtxclrlock(mtx)	(void)(-1)

#define vtstatus(vt)		((void*)0)
#define vterror(vt)		(0)
#define vtmtxerror(mtx)		(0)
#define vtonceerror(once)	(0)

#endif				/*!vt_threaded */

#endif /*_VTHREAD_H*/

#ifdef __cplusplus
}
#endif
