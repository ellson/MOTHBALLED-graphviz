/* $Id$ $Revision$ */
/* vim:set shiftwidth=4 ts=8: */

/*************************************************************************
 * Copyright (c) 2011 AT&T Intellectual Property 
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors: See CVS logs. Details at http://www.graphviz.org/
 *************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#ifndef         AGXBUF_H
#define         AGXBUF_H

#ifdef _WIN32
#   ifdef EXPORT_AGXBUF
#       define AGXBUF_API __declspec(dllexport)
#   else
#       define AGXBUF_API __declspec(dllimport)
#   endif
#else
#   define AGXBUF_API extern
#endif

/* Extensible buffer:
 *  Malloc'ed memory is never released until agxbfree is called.
 */
    typedef struct {
	unsigned char *buf;	/* start of buffer */
	unsigned char *ptr;	/* next place to write */
	unsigned char *eptr;	/* end of buffer */
	int dyna;		/* true if buffer is malloc'ed */
    } agxbuf;

/* agxbinit:
 * Initializes new agxbuf; caller provides memory.
 * Assume if init is non-null, hint = sizeof(init[])
 */
    AGXBUF_API void agxbinit(agxbuf * xb, unsigned int hint,
			 unsigned char *init);

/* agxbput_n:
 * Append string s of length n into xb
 */
    AGXBUF_API size_t agxbput_n(agxbuf * xb, const char *s, size_t n);

/* agxbput:
 * Append string s into xb
 */
    AGXBUF_API size_t agxbput(agxbuf * xb, const char *s);

/* agxbfree:
 * Free any malloced resources.
 */
    AGXBUF_API void agxbfree(agxbuf * xb);

/* agxbpop:
 * Removes last character added, if any.
 */
    AGXBUF_API int agxbpop(agxbuf * xb);

/* agxbmore:
 * Expand buffer to hold at least ssz more bytes.
 */
    AGXBUF_API int agxbmore(agxbuf * xb, size_t ssz);

/* agxbputc:
 * Add character to buffer.
 *  int agxbputc(agxbuf*, char)
 */
#define agxbputc(X,C) ((((X)->ptr >= (X)->eptr) ? agxbmore(X,1) : 0), (void)(*(X)->ptr++ = ((unsigned char)C)))

/* agxbuse:
 * Null-terminates buffer; resets and returns pointer to data;
 *  char* agxbuse(agxbuf* xb)
 */
#define agxbuse(X) ((void)agxbputc(X,'\0'),(char*)((X)->ptr = (X)->buf))

/* agxbstart:
 * Return pointer to beginning of buffer.
 *  char* agxbstart(agxbuf* xb)
 */
#define agxbstart(X) ((char*)((X)->buf))

/* agxblen:
 * Return number of characters currently stored.
 *  int agxblen(agxbuf* xb)
 */
#define agxblen(X) (((X)->ptr)-((X)->buf))

/* agxbclear:
 * Resets pointer to data;
 *  void agxbclear(agxbuf* xb)
 */
#define agxbclear(X) ((void)((X)->ptr = (X)->buf))

/* agxbnext:
 * Next position for writing.
 *  char* agxbnext(agxbuf* xb)
 */
#define agxbnext(X) ((char*)((X)->ptr))

#endif

#ifdef __cplusplus
}
#endif
