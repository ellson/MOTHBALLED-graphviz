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

/* this is to get the following win32 DLL junk to work.
 * if ever tempted to remove this, first please read:
 * http://joel.editthispage.com/stories/storyReader$47
 */
#ifndef GLOBALS_H
#define GLOBALS_H

#ifdef _UWIN
#ifndef _POSIX_			/* ncc doesn't define _POSIX_ */
/* i.e. if this is the win32 build using nmake with CC=ncc (native C) */
/* this was the easiest way to get some simple libc interfaces. */
#include "C:\Program Files\UWIN\usr\include\astwin32.h"
#undef _UWIN			/* don't assume ANY _UWIN features in the execution environment */
#endif				/* _POSIX_ */
#endif				/* _UWIN */

#ifndef __CYGWIN__
#if defined(_BLD_dotneato) && defined(_DLL)
#   define external __EXPORT__
#endif
#if !defined(_BLD_dotneato) && defined(__IMPORT__)
#   define external __IMPORT__
#endif
#endif

#ifndef external
#   define external   extern
#endif
#ifndef EXTERN
#define EXTERN extern
#endif

#ifdef __cplusplus
extern "C" {
#endif

    EXTERN char *Version;
    EXTERN char **Files;	/* from command line */
    EXTERN char **Lib;		/* from command line */
    EXTERN char *CmdName;
    EXTERN char *specificFlags;
    EXTERN char *specificItems;
    external char *Gvfilepath;	/* Path of files allowed in 'shapefile' attrib  (also ps libs) */

#ifndef DISABLE_CODEGENS
    EXTERN int Output_lang;	/* POSTSCRIPT, DOT, etc. */
    EXTERN FILE *Output_file;
    EXTERN int Obj;

    extern codegen_t FIG_CodeGen, HPGL_CodeGen, MAP_CodeGen,
	MIF_CodeGen, XDot_CodeGen, MP_CodeGen, PIC_CodeGen,
	PS_CodeGen, DIA_CodeGen, SVG_CodeGen, VRML_CodeGen,
	VTX_CodeGen, GD_CodeGen, memGD_CodeGen;
#ifdef QUARTZ_RENDER
    extern codegen_t QPDF_CodeGen, QEPDF_CodeGen, QBM_CodeGen;
#endif
#endif

    EXTERN boolean Verbose, Reduce, UseRankdir, MemTest;
    EXTERN char *HTTPServerEnVar;
    EXTERN char *Output_file_name;
    EXTERN int graphviz_errors;
    EXTERN int Nop;
    EXTERN double PSinputscale;
    EXTERN int Syntax_errors;
    EXTERN int Show_cnt;
    EXTERN char** Show_boxes;	/* emit code for correct box coordinates */
    EXTERN int CL_type;		/* NONE, LOCAL, GLOBAL */
    EXTERN boolean Concentrate;	/* if parallel edges should be merged */
    EXTERN double Epsilon;	/* defined in input_graph */
    EXTERN double Nodesep;
    EXTERN double Nodefactor;
    EXTERN int MaxIter;
    EXTERN int Ndim;
    EXTERN int State;		/* last finished phase */
    EXTERN double Initial_dist;
    EXTERN double Damping;
    EXTERN int Y_invert;	/* invert y in dot & plain output */
    EXTERN int Y_off;           /* ymin + ymax */
    EXTERN double YF_off;       /* Y_off in inches */
    EXTERN attrsym_t *g_draw;
    EXTERN attrsym_t *g_l_draw;

    EXTERN attrsym_t
	*G_activepencolor, *G_activefillcolor,
	*G_peripheries;
    EXTERN attrsym_t
	*N_height, *N_width, *N_shape, *N_color, *N_fillcolor,
	*N_activepencolor, *N_activefillcolor,
	*N_fontsize, *N_fontname, *N_fontcolor,
	*N_label, *N_nojustify, *N_style, *N_showboxes,
	*N_sides, *N_peripheries, *N_orientation,
	*N_skew, *N_distortion, *N_fixed, *N_layer,
	*N_group, *N_comment, *N_vertices, *N_z;
    EXTERN attrsym_t
	*E_weight, *E_minlen, *E_color,
	*E_activepencolor, *E_activefillcolor,
	*E_fontsize, *E_fontname, *E_fontcolor,
	*E_label, *E_dir, *E_style, *E_decorate,
	*E_showboxes, *E_arrowsz, *E_constr, *E_layer,
	*E_comment, *E_label_float,
	*E_samehead, *E_sametail,
	*E_arrowhead, *E_arrowtail,
	*E_headlabel, *E_taillabel,
	*E_labelfontsize, *E_labelfontname, *E_labelfontcolor,
	*E_labeldistance, *E_labelangle,
	*E_tailclip, *E_headclip;

    EXTERN fdpParms_t fdp_parms;

#undef external

#ifdef __cplusplus
}
#endif
#endif
