/* vim:set shiftwidth=4 ts=8: */
#ifndef INTSET_H
#define INTSET_H

#include <cdt.h>

typedef struct {
    int       id;
    Dtlink_t  link;
} intitem;

extern Dt_t* openIntSet ();
#endif
