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

/* Common header used by both clients and plugins */

#ifndef GVCEXT_H
#define GVCEXT_H

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct gvdevice_engine_s gvdevice_engine_t;
    typedef struct gvrender_engine_s gvrender_engine_t;
    typedef struct gvlayout_engine_s gvlayout_engine_t;
    typedef struct gvtextlayout_engine_s gvtextlayout_engine_t;
    typedef struct gvusershape_engine_s gvusershape_engine_t;

    typedef struct GVJ_s GVJ_t;
    typedef struct GVC_s GVC_t;

#ifndef DISABLE_CODEGENS
    typedef struct codegen_s codegen_t;
    typedef struct codegen_info_s codegen_info_t;
#endif

#ifdef __cplusplus
}
#endif

#endif
