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

/*
 * Glenn Fowler
 * AT&T Research
 *
 * expression library readonly tables
 */

static const char id[] =
    "\n@(#)$Id$\0\n";

#include <exlib.h>

const char *exversion = id + 10;

Exid_t exbuiltin[] = {

    /* id_string references the first entry */

    EX_ID("string", DECLARE, STRING, STRING, 0),

    /* order not important after this point (but sorted anyway) */

    EX_ID("break", BREAK, BREAK, 0, 0),
    EX_ID("case", CASE, CASE, 0, 0),
    EX_ID("char", DECLARE, CHAR, CHAR, 0),
    EX_ID("continue", CONTINUE, CONTINUE, 0, 0),
    EX_ID("default", DEFAULT, DEFAULT, 0, 0),
    EX_ID("double", DECLARE, FLOATING, FLOATING, 0),
    EX_ID("else", ELSE, ELSE, 0, 0),
    EX_ID("exit", EXIT, EXIT, INTEGER, 0),
    EX_ID("for", FOR, FOR, 0, 0),
    EX_ID("float", DECLARE, FLOATING, FLOATING, 0),
    EX_ID("gsub", GSUB, GSUB, STRING, 0),
    EX_ID("if", IF, IF, 0, 0),
    EX_ID("int", DECLARE, INTEGER, INTEGER, 0),
    EX_ID("long", DECLARE, INTEGER, INTEGER, 0),
    EX_ID("print", PRINT, PRINT, INTEGER, 0),
    EX_ID("printf", PRINTF, PRINTF, INTEGER, 0),
    EX_ID("query", QUERY, QUERY, INTEGER, 0),
    EX_ID("rand", RAND, RAND, FLOATING, 0),
    EX_ID("return", RETURN, RETURN, 0, 0),
    EX_ID("scanf", SCANF, SCANF, INTEGER, 0),
    EX_ID("sscanf", SSCANF, SSCANF, INTEGER, 0),
    EX_ID("sprintf", SPRINTF, SPRINTF, STRING, 0),
    EX_ID("srand", SRAND, SRAND, INTEGER, 0),
    EX_ID("sub", SUB, SUB, STRING, 0),
    EX_ID("substr", SUBSTR, SUBSTR, STRING, 0),
    EX_ID("switch", SWITCH, SWITCH, 0, 0),
    EX_ID("unsigned", DECLARE, UNSIGNED, UNSIGNED, 0),
    EX_ID("void", DECLARE, VOID, 0, 0),
    EX_ID("while", WHILE, WHILE, 0, 0),
    EX_ID({0}, 0, 0, 0, 0)

};
