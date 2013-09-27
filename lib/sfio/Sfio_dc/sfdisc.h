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

#ifndef _SFDISC_H
#define _SFDISC_H	1

#include	<sfio.h>

    _BEGIN_EXTERNS_
/* functions to create disciplines */
    extern int sfdcdio _ARG_((Sfio_t *, size_t));
    extern int sfdcdos _ARG_((Sfio_t *));
    extern int sfdcfilter _ARG_((Sfio_t *, const char *));
    extern int sfdclzw _ARG_((Sfio_t *));
    extern int sfdcseekable _ARG_((Sfio_t *));
    extern int sfdcslow _ARG_((Sfio_t *));
    extern int sfdcsubstream _ARG_((Sfio_t *, Sfio_t *, Sfoff_t, Sfoff_t));
    extern int sfdctee _ARG_((Sfio_t *, Sfio_t *));
    extern int sfdcunion _ARG_((Sfio_t *, Sfio_t **, int));
     _END_EXTERNS_
#endif
#ifdef __cplusplus
}
#endif
