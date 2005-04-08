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

#ifdef __cplusplus
extern "C" {
#endif

/*  File - TrieFA.h
    The data types for the generated trie-baseed finite automata.
*/

struct TrieState {  /* An entry in the FA state table */
    short def;         /* If this state is an accepting state then */
                       /* this is the definition, otherwise -1.    */
    short trans_base;  /* The base index into the transition table.*/
    long  mask;        /* The transition mask.                     */
};

struct TrieTrans {  /* An entry in the FA transition table. */
    short c;           /* The transition character (lowercase). */
    short next_state;  /* The next state.                       */
};

typedef struct TrieState TrieState;
typedef struct TrieTrans TrieTrans;

extern TrieState TrieStateTbl[];
extern TrieTrans TrieTransTbl[];

#ifdef __cplusplus
}
#endif

