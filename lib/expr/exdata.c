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

    EXID("string", DECLARE, STRING, STRING, 0),

    /* order not important after this point (but sorted anyway) */

    EXID("break", BREAK, BREAK, 0, 0),
    EXID("case", CASE, CASE, 0, 0),
    EXID("char", DECLARE, CHAR, CHAR, 0),
    EXID("continue", CONTINUE, CONTINUE, 0, 0),
    EXID("default", DEFAULT, DEFAULT, 0, 0),
    EXID("double", DECLARE, FLOATING, FLOATING, 0),
    EXID("else", ELSE, ELSE, 0, 0),
    EXID("exit", EXIT, EXIT, INTEGER, 0),
    EXID("for", FOR, FOR, 0, 0),
    EXID("float", DECLARE, FLOATING, FLOATING, 0),
    EXID("gsub", GSUB, GSUB, STRING, 0),
    EXID("if", IF, IF, 0, 0),
    EXID("int", DECLARE, INTEGER, INTEGER, 0),
    EXID("long", DECLARE, INTEGER, INTEGER, 0),
    EXID("print", PRINT, PRINT, INTEGER, 0),
    EXID("printf", PRINTF, PRINTF, INTEGER, 0),
    EXID("query", QUERY, QUERY, INTEGER, 0),
    EXID("rand", RAND, RAND, FLOATING, 0),
    EXID("return", RETURN, RETURN, 0, 0),
    EXID("scanf", SCANF, SCANF, INTEGER, 0),
    EXID("sscanf", SSCANF, SSCANF, INTEGER, 0),
    EXID("sprintf", SPRINTF, SPRINTF, STRING, 0),
    EXID("srand", SRAND, SRAND, INTEGER, 0),
    EXID("sub", SUB, SUB, STRING, 0),
    EXID("substr", SUBSTR, SUBSTR, STRING, 0),
    EXID("switch", SWITCH, SWITCH, 0, 0),
    EXID("unsigned", DECLARE, UNSIGNED, UNSIGNED, 0),
    EXID("void", DECLARE, VOID, 0, 0),
    EXID("while", WHILE, WHILE, 0, 0),
    EXID({0}, 0, 0, 0, 0)

};
