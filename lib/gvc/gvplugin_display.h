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

#ifndef GVDISPLAY_PLUGIN_H
#define GVDISPLAY_PLUGIN_H

#include "gvplugin.h"
#include "gvcint.h"

#ifdef __cplusplus
extern "C" {
#endif

    struct gvdisplay_engine_s {
	void (*init) (GVC_t * gvc);
	void (*process) (GVC_t * gvc);
	void (*deinit) (GVC_t * gvc);
    };

/* event callbacks in gvevent.c */

    extern void gvevent_refresh(gvrender_job_t * job);
    extern void gvevent_button_press(gvrender_job_t * job, XButtonEvent *bev);
    extern void gvevent_motion(gvrender_job_t * job, XMotionEvent *mev);
    extern void gvevent_button_release(gvrender_job_t *job, XButtonEvent *bev);
    extern int gvevent_key_press(gvrender_job_t * job, XKeyEvent * kev);
    extern void gvevent_reconfigure_normal(gvrender_job_t * job, unsigned int width, unsigned int height);
    extern void gvevent_reconfigure_fit_mode(gvrender_job_t * job, unsigned int width, unsigned int height);
    extern void gvevent_configure(gvrender_job_t * job, XConfigureEvent * cev);
    extern void gvevent_expose(gvrender_job_t * job, XExposeEvent * eev);
    extern void gvevent_client_message(gvrender_job_t * job, XClientMessageEvent * cmev);

#ifdef __cplusplus
}
#endif
#endif				/* GVDISPLAY_PLUGIN_H */
