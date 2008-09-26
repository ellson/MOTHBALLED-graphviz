/* $Id$ $Revision$ */
/* vim:set shiftwidth=4 ts=8: */

/***********************************************************
 *      This software is part of the graphviz package      *
 *                http://www.graphviz.org/                 *
 *                                                         *
 *            Copyright (c) 1994-2008 AT&T Corp.           *
 *                and is licensed under the                *
 *            Common Public License, Version 1.0           *
 *                      by AT&T Corp.                      *
 *                                                         *
 *        Information and Software Systems Research        *
 *              AT&T Research, Florham Park NJ             *
 **********************************************************/

#ifndef INKPOT_XLATE_H
#define INKPOT_XLATE_H

#ifdef __cplusplus
extern "C" {
#endif

extern void hsva2rgba(double hsva[4], double rgba[4]);
extern void rgba2hsva(double rgba[4], double hsva[4]);
extern void rgba2cmyk(double rgba[4], double cmyk[4]);

#ifdef __cplusplus
}
#endif
#endif /* INKPOT_XLATE_H */
