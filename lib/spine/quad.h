#ifndef QUAD_H
#define QUAD_H

#include <cgraph.h>

typedef void (*quadfn_t)(Agnode_t*, Agnode_t*, Dt_t*, void*);

extern void genQuads (Agraph_t*, quadfn_t action, void*);

#endif
