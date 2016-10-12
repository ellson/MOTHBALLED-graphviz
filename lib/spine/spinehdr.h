#ifndef SPINEHDR_T
#define SPINEHDR_T

/*
#include <stdlib.h>
#include <math.h>
#include <subset.h>
#include <assert.h>
*/
#include <spine.h>

#define N_NEW(n,t)       (t*)mcalloc((n),sizeof(t))
#define NEW(t)       (t*)mcalloc((1),sizeof(t))

extern void *mcalloc(size_t nmemb, size_t size);

#define NOTUSED(var)     (void) var

typedef struct {
	Agrec_t h;
	int id;
	int deg;
	int UF_size;
	Agnode_t *UF_parent;
} nodeinfo_t;

typedef struct {
	Agrec_t h;
	float weight;
	int id;
} edgeinfo_t;

#define ED_wt(e) (((edgeinfo_t*)AGDATA(e))->weight)
#define ED_id(e) (((edgeinfo_t*)AGDATA(e))->id)
#define ND_id(n) (((nodeinfo_t*)AGDATA(n))->id)
#define ND_deg(n) (((nodeinfo_t*)AGDATA(n))->deg)
#define ND_UF_parent(n) (((nodeinfo_t*)AGDATA(n))->UF_parent)
#define ND_UF_size(n) (((nodeinfo_t*)AGDATA(n))->UF_size)

#endif
