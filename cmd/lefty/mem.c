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


/* Lefteris Koutsofios - AT&T Bell Laboratories */

#include "common.h"
#include "mem.h"

/* SHORTCUT: the following are imported from tbl.c */
extern void Tgchelper(void *), Tfreehelper(void *);

#define M_MAXTYPES 6

int Mhaspointers[M_MAXTYPES];
Mgcstate_t Mgcstate;
int Mcouldgc;

typedef struct buffer_t {
    void *data;
    struct buffer_t *next;
} buffer_t;
#define BUFFERSIZE sizeof (buffer_t)
static buffer_t *bufferlist;

typedef struct freeobj_t {
    Mheader_t head;
    void *next;
} freeobj_t;
#define FREEOBJSIZE sizeof (freeobj_t)
#define FREESIZE sizeof (void *)
static void **freearray;
static long freen;

#define MARKSIZE sizeof (void *)
#define MARKINCR 100
static void **markarray;
static long markn, marki;

#define OTSIZE sizeof (void *)
#define OTINCR 1000
static void **otarray[2];
static long otn, oti, otj;
static char otarea[2];

static void (*cbfunc) (void);

#define GCINCRSTEPS 4
static int gcsteps = GCINCRSTEPS;

#define NUMOFALLOCS 10
static int numofallocs = 0;

#define CALCNUM(size) ((size < 20) ? 1000 : ((size < 200) ? 100 : 1))

#ifdef STATS
static caddr_t origsbrk, finsbrk;
static long objnum, objsiz;
static long typenum[M_MAXTYPES], typesiz[M_MAXTYPES];
#endif

static void allocbuffer(long);

void Minit(void (*func) (void))
{

#ifdef STATS
    int i;

    origsbrk = (caddr_t) sbrk(0);
    for (i = 0; i < M_MAXTYPES; i++)
	typenum[i] = typesiz[i] = 0;
#endif

    freearray = Marrayalloc((long) FREESIZE);
    freen = 1;
    freearray[0] = NULL;
    markarray = Marrayalloc((long) MARKINCR * MARKSIZE);
    markn = MARKINCR;
    marki = 0;
    otarray[0] = Marrayalloc((long) OTINCR * OTSIZE);
    otarray[1] = Marrayalloc((long) OTINCR * OTSIZE);
    otn = OTINCR;
    oti = otj = 0;
    otarea[0] = 1, otarea[1] = 2;
    cbfunc = func;
    Mcouldgc = FALSE;
}

void Mterm(void)
{
    buffer_t *bp;

#ifdef STATS
    finsbrk = (caddr_t) sbrk(0);
    printf("memory used by data: %ld\n", (long) (finsbrk - origsbrk));
#endif

    Marrayfree(otarray[0]), Marrayfree(otarray[1]);
    otarray[0] = otarray[1] = NULL;
    otn = otj = oti = 0;
    Marrayfree(freearray), freearray = NULL, freen = 0;
    Marrayfree(markarray), markarray = NULL, markn = marki = 0;
    for (bp = bufferlist; bp;) {
	free(bp->data);
	bufferlist = bp, bp = bp->next, free(bufferlist);
    }
    bufferlist = NULL;
}

void *Mnew(long size, int type)
{
    freeobj_t *fp;

    size = (size < FREEOBJSIZE) ?
	M_BYTE2SIZE(FREEOBJSIZE) : M_BYTE2SIZE(size);
    if (size >= freen || !freearray[size]) {
	allocbuffer(size), numofallocs++;
	if (numofallocs == NUMOFALLOCS)
	    Mdogc(M_GCFULL), gcsteps <<= 1, numofallocs = 0;
	else
	    Mdogc(M_GCINCR);
    } else if (Mgcstate == M_GCON)
	Mdogc(M_GCINCR);
    fp = freearray[size], freearray[size] = fp->next;
    fp->head.type = type;
    Mmkcurr(fp);
    Mcouldgc = TRUE;

#ifdef STATS
    objnum++, objsiz += size;
    typenum[type]++, typesiz[type] += size;
#endif

    return fp;
}

void *Mallocate(long size)
{
    freeobj_t *fp;

    size = (size < FREEOBJSIZE) ?
	M_BYTE2SIZE(FREEOBJSIZE) : M_BYTE2SIZE(size);
    if (size >= freen || !freearray[size])
	allocbuffer(size);
    fp = freearray[size], freearray[size] = fp->next;
    return fp;
}

void Mfree(void *p, long size)
{
    freeobj_t *fp;

    fp = p;
    fp->head.size = size;
    fp->head.area = 0;
    fp->head.type = 0;
    fp->next = freearray[fp->head.size], freearray[fp->head.size] = fp;
}

#ifndef FEATURE_MS

void *Marrayalloc(long size)
{
    void *p;

    if (!(p = malloc(size)))
	panic(POS, "Marrayallocate", "cannot allocate array");
    return p;
}

void *Marraygrow(void *p, long size)
{
    if (!(p = realloc(p, size)))
	panic(POS, "Marrayreallocate", "cannot re-allocate array");
    return p;
}

void Marrayfree(void *p)
{
    free(p);
}

#else

static struct arraymap_t {
    HGLOBAL k;
    void *v;
} arraymap[100];

void *Marrayalloc(long size)
{
    int i;

    for (i = 0; i < 100; i++)
	if (!arraymap[i].k)
	    break;
    if (i == 100)
	panic(POS, "Marrayalloc", "out of space in arraymap");
    if (!(arraymap[i].k = GlobalAlloc(GPTR, size)))
	panic(POS, "Marrayallocate", "cannot allocate array");
    arraymap[i].v = GlobalLock(arraymap[i].k);
    return arraymap[i].v;
}

void *Marraygrow(void *p, long size)
{
    int i;

    for (i = 0; i < 100; i++)
	if (arraymap[i].v == p)
	    break;
    if (i == 100)
	panic(POS, "Marraygrow", "cannot locate pointer");
    if (!
	(arraymap[i].k =
	 GlobalReAlloc(arraymap[i].k, size, GMEM_MOVEABLE)))
	panic(POS, "Marraygrow", "cannot re-allocate array");
    arraymap[i].v = GlobalLock(arraymap[i].k);
    return arraymap[i].v;
}

void Marrayfree(void *p)
{
    int i;

    for (i = 0; i < 100; i++)
	if (arraymap[i].v == p)
	    break;
    if (i == 100)
	panic(POS, "Marrayfree", "cannot locate pointer");
    GlobalUnlock(arraymap[i].k);
    GlobalFree(arraymap[i].k);
    arraymap[i].k = 0;
}

#endif

long Mpushmark(void *p)
{
    if (marki == markn) {
	markarray = Marraygrow(markarray,
			       (long) (markn + MARKINCR) * MARKSIZE);
	markn += MARKINCR;
    }
    markarray[marki++] = p;
    if (Mgcstate == M_GCON)
	Mmkcurr(p);
    return marki - 1;
}

void Mpopmark(long m)
{
    marki = m;
}

void Mresetmark(long m, void *p)
{
    markarray[m] = p;
    if (Mgcstate == M_GCON)
	Mmkcurr(p);
}

void Mmkcurr(void *p)
{
    if (!p || M_AREAOF(p) == otarea[0])
	return;
    if (oti >= otn) {
	otarray[0] =
	    Marraygrow(otarray[0], (long) (otn + OTINCR) * OTSIZE);
	otarray[1] =
	    Marraygrow(otarray[1], (long) (otn + OTINCR) * OTSIZE);
	otn += OTINCR;
    }
    otarray[0][oti++] = p;
    ((Mheader_t *) p)->area = otarea[0];
}

void Mdogc(Mgctype_t gctype)
{
    void *p;
    long i;
    char n;
    static long prevoti;

    if (Mgcstate == M_GCOFF) {
	Mgcstate = M_GCON;
	p = otarray[0], otarray[0] = otarray[1], otarray[1] = p;
	n = otarea[0], otarea[0] = otarea[1], otarea[1] = n;
	prevoti = oti, oti = otj = 0;
	for (i = 0; i < marki; i++)
	    Mmkcurr(markarray[i]);
    }
    if (gctype == M_GCFULL) {
	while (otj != oti) {
	    p = otarray[0][otj];
	    if (Mhaspointers[M_TYPEOF(p)])
		Tgchelper(p);
	    otj++;
	}
    } else {
	for (i = 0; i < gcsteps && otj != oti; i++) {
	    p = otarray[0][otj];
	    if (Mhaspointers[M_TYPEOF(p)])
		Tgchelper(p);
	    otj++;
	}
	if (otj < oti)
	    return;
    }
    for (i = 0; i < prevoti; i++) {
	p = otarray[1][i];
	if (p && M_AREAOF(p) == otarea[1]) {
	    if (Mhaspointers[M_TYPEOF(p)])
		Tfreehelper(p);
	    Mfree(p, (long) ((Mheader_t *) p)->size);
	}
    }
    if (gctype == M_GCINCR) {
	if (numofallocs < NUMOFALLOCS / 2) {
	    int t = (gcsteps > GCINCRSTEPS) ? gcsteps >>= 1 : GCINCRSTEPS;
	    gcsteps = t;
	    numofallocs = 0;
	}
    }
    if (cbfunc)
	(*cbfunc) ();
    Mgcstate = M_GCOFF;
    Mcouldgc = FALSE;
}

void Mreport(void)
{
    Mheader_t *p;
    long num[M_MAXTYPES], siz[M_MAXTYPES];
    long i, n;
    freeobj_t *fp;

    Mdogc(M_GCFULL);
    Mdogc(M_GCFULL);
    for (i = 0; i < M_MAXTYPES; i++)
	num[i] = siz[i] = 0;
    for (i = 0; i < oti; i++) {
	p = otarray[0][i];
	siz[0] += p->size;
	siz[M_TYPEOF(p)] += p->size;
	num[M_TYPEOF(p)]++;
	num[0]++;
    }
    fprintf(stderr, "live objects:      %8ld (", num[0]);
    for (i = 1; i < M_MAXTYPES; i++)
	fprintf(stderr, "%8ld%s", num[i],
		(i == M_MAXTYPES - 1) ? "" : ",");
    fprintf(stderr, ")\n       sizes:      %8ld (", siz[0]);
    for (i = 1; i < M_MAXTYPES; i++)
	fprintf(stderr, "%8ld%s", siz[i],
		(i == M_MAXTYPES - 1) ? "" : ",");
    fprintf(stderr, ")\n");
    fprintf(stderr, "free lists: %ld\n", freen);
    for (i = 0; i < freen; i++) {
	for (n = 0, fp = freearray[i]; fp; fp = fp->next)
	    n++;
	if (n > 0)
	    fprintf(stderr, "free list: %ld - %ld\n", i, n);
    }
#ifdef STATS
    printf("objects allocated: %8ld (", objnum);
    for (i = 1; i < M_MAXTYPES; i++)
	printf("%l8d%s", typenum[i], (i == M_MAXTYPES - 1) ? "" : ",");
    printf(")\n            sizes: %8ld (", objsiz);
    for (i = 1; i < M_MAXTYPES; i++)
	printf("%8ld%s", typesiz[i], (i == M_MAXTYPES - 1) ? "" : ",");
    printf(")\n");
    finsbrk = (caddr_t) sbrk(0);
    printf("memory used by data: %ld\n", (long) (finsbrk - origsbrk));
#endif
}

static void allocbuffer(long size)
{
    buffer_t *bp;
    char *p;
    long i, bytes, n;

    if (size >= freen) {
	if (size > M_SIZEMAX)
	    panic(POS, "allocbuffer", "size is too big");
	freearray = Marraygrow(freearray, (long) (size + 1) * FREESIZE);
	for (i = freen; i < size + 1; i++)
	    freearray[i] = NULL;
	freen = size + 1;
    }
    n = CALCNUM(size);
    if (!freearray[size]) {
	if (!(bp = malloc(BUFFERSIZE)))
	    panic(POS, "allocbuffer", "cannot allocate buffer struct");
	if (!(bp->data = malloc(size * M_UNITSIZE * n)))
	    panic(POS, "allocbuffer", "cannot allocate buffer");
	bp->next = bufferlist, bufferlist = bp;
	bytes = size * M_UNITSIZE;
	for (i = 0, p = bp->data; i < n - 1; i++, p += bytes) {
	    ((freeobj_t *) p)->next = p + bytes;
	    ((freeobj_t *) p)->head.size = (Msize_t) size;
	    ((freeobj_t *) p)->head.area = 0;
	    ((freeobj_t *) p)->head.type = 0;
	}
	((freeobj_t *) p)->next = NULL;
	((freeobj_t *) p)->head.size = (Msize_t) size;
	((freeobj_t *) p)->head.area = 0;
	((freeobj_t *) p)->head.type = 0;
	freearray[size] = bp->data;
    }
}
