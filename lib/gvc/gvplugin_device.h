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

#ifndef GVDEVICE_PLUGIN_H
#define GVDEVICE_PLUGIN_H

#include "gvplugin.h"
#include "gvcint.h"

#ifdef __cplusplus
extern "C" {
#endif

    struct gvdevice_engine_s {
	void (*initialize) (GVC_t * gvc, gvevent_key_binding_t *keybindings, int numkeys);
	void (*finalize) (GVC_t * gvc);
	void (*begin_job) (GVJ_t * job);
        void (*end_job) (GVJ_t * job);
    };

/* callbacks */
    extern void gvevent_refresh(GVJ_t * job);
    extern void gvevent_button_press(GVJ_t * job, int button, double x, double y);
    extern void gvevent_button_release(GVJ_t * job, int button, double x, double y);
    extern void gvevent_motion(GVJ_t * job, double x, double y);


#ifdef __cplusplus
}
#endif
#endif				/* GVDEVICE_PLUGIN_H */
