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


#ifndef         NEATO_H
#define         NEATO_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define MODEL_SHORTPATH      0
#define MODEL_CIRCUIT        1
#define MODEL_SUBSET         2

#define MODE_KK          0
#define MODE_MAJOR       1

#define INIT_SELF        0
#define INIT_REGULAR     1
#define INIT_RANDOM      2

#include	"render.h"
#include	"pathplan.h"
#include	"neatoprocs.h"
#include	"adjust.h"

#endif				/* NEATO_H */
