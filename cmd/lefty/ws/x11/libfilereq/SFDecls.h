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

/* SFDecls.h */

/* We don't rely on compiler doing the right thing
 * in absence of declarations.
 * C compilers should never have accepted this braindamage.
 * KG <garloff@suse.de>, 2002-01-28
 */

/* SelFile.c */
FILE * SFopenFile (char *, char *, char *, char *);
void SFtextChanged (void);

/* Draw.c */
void SFinitFont (void);
void SFcreateGC (void);
void SFclearList (int, int);
void SFdrawList (int, int);
void SFdrawLists (int);

/* Path.c */
int SFchdir (char *path);
void SFupdatePath (void);
void SFsetText (char *path);

/* Dir.c */
int SFcompareEntries (const void *vp, const void *vq);
int SFgetDir (SFDir *dir);

#ifdef __cplusplus
}
#endif

