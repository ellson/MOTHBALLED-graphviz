#ifndef SUBSET_H
#define SUBSET_H

typedef struct {
    Dtlink_t  link;
    void* v;
} ptritem;

typedef int (*walkfn)(void*, void*);

extern Dt_t* mkSubset(void);
extern void addSubset(Dt_t*, void*);
extern void walkSubset(Dt_t*, walkfn, void*);
extern int sizeSubset(Dt_t*);
extern void clearSubset(Dt_t*);
extern void closeSubset(Dt_t*);

#endif
