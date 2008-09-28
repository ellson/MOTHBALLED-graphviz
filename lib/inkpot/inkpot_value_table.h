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

inkpot_value_t TAB_VALUES[SZT_VALUES] = { /* Must be sort'ed */
    { 0x000000000000ffff, 0 },  /* black */
    { 0x00000000ffffffff, 1 },  /* bleu, blue */
    { 0x0000ffff0000ffff, 3 },  /* green, vert */
    { 0xffff00000000ffff, 5 },  /* red, rouge */
    { 0xffffcf000000ffff, 7 },  /* yellow (svg) */
    { 0xffffffff0000ffff, 8 },  /* yellow (x11), jaune */
    { 0xffffffffffffffff, 10},  /* white */
}; 

inkpot_noname_value_t TAB_NONAME_VALUES[SZT_NONAME_VALUES] = { /* Must be sort'ed */
    { 0x000000008000ffff, },
    { 0x00000000a400ffff, },
    { 0x00000000c800ffff, },
    { 0x00000000ea00ffff, },
}; 

IDX_VALUES TAB_IXVALUES[SZT_IXVALUES] = {
    7, 8, 9, 10, 1,  /* xxx */
    0, 6,            /* yyy */
};
