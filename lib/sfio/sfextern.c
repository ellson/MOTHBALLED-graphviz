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

#include	"sfhdr.h"

/*	External variables and functions used only by Sfio
**	Written by Kiem-Phong Vo
*/

/* code to initialize mutexes */
static Vtmutex_t Sfmutex;
static Vtonce_t Sfonce = VTONCE_INITDATA;
static void _sfoncef(void)
{
    vtmtxopen(_Sfmutex, VT_INIT);
    vtmtxopen(&_Sfpool.mutex, VT_INIT);
    vtmtxopen(sfstdin->mutex, VT_INIT);
    vtmtxopen(sfstdout->mutex, VT_INIT);
    vtmtxopen(sfstderr->mutex, VT_INIT);
    _Sfdone = 1;
}

/* global variables used internally to the package */
Sfextern_t _Sfextern = { 0,	/* _Sfpage      */
    {NIL(Sfpool_t *), 0, 0, 0, NIL(Sfio_t **)},	/* _Sfpool      */
    NIL(int (*)(Sfio_t *, int)),	/* _Sfpmove     */
    NIL(Sfio_t * (*)(Sfio_t *, Sfio_t *)),	/* _Sfstack     */
    NIL(void (*)(Sfio_t *, int, int)),	/* _Sfnotify    */
    NIL(int (*)(Sfio_t *)),	/* _Sfstdsync   */
    {NIL(Sfread_f),		/* _Sfudisc     */
     NIL(Sfwrite_f),
     NIL(Sfseek_f),
     NIL(Sfexcept_f),
     NIL(Sfdisc_t *)
     },
    NIL(void (*)(void)),	/* _Sfcleanup   */
    0,				/* _Sfexiting   */
    0,				/* _Sfdone      */
    &Sfonce,			/* _Sfonce      */
    _sfoncef,			/* _Sfoncef     */
    &Sfmutex			/* _Sfmutex     */
};

/* accessible to application code for a few fast macro functions */
ssize_t _Sfi = -1;

#if vt_threaded
static Vtmutex_t _Sfmtxin, _Sfmtxout, _Sfmtxerr;
#define SFMTXIN		(&_Sfmtxin)
#define SFMTXOUT	(&_Sfmtxout)
#define SFMTXERR	(&_Sfmtxerr)
#else
#define SFMTXIN		(0)
#define SFMTXOUT	(0)
#define SFMTXERR	(0)
#endif

Sfio_t _Sfstdin = SFNEW(NIL(char *), -1, 0,
			(SF_READ | SF_STATIC | SF_MTSAFE), NIL(Sfdisc_t *),
			SFMTXIN);
Sfio_t _Sfstdout = SFNEW(NIL(char *), -1, 1,
			 (SF_WRITE | SF_STATIC | SF_MTSAFE),
			 NIL(Sfdisc_t *), SFMTXOUT);
Sfio_t _Sfstderr = SFNEW(NIL(char *), -1, 2,
			 (SF_WRITE | SF_STATIC | SF_MTSAFE),
			 NIL(Sfdisc_t *), SFMTXERR);

Sfio_t *sfstdin = &_Sfstdin;
Sfio_t *sfstdout = &_Sfstdout;
Sfio_t *sfstderr = &_Sfstderr;

extern ssize_t _Sfi;
extern Sfio_t _Sfstdin;
extern Sfio_t _Sfstdout;
extern Sfio_t _Sfstderr;
extern Sfio_t *sfstdin;
extern Sfio_t *sfstdout;
extern Sfio_t *sfstderr;
