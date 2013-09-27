#include "newdot.h"

/*
 *  a queue of nodes
 */
queue *
new_queue(int sz)
{
    queue       *q = NEW(queue);
                                                                                
    if (sz <= 1) sz = 2;
    q->head = q->tail = q->store = N_NEW(sz,node_t*);
    q->limit = q->store + sz;
    return q;
}
                                                                                
void
free_queue(queue* q)
{
    free(q->store);
    free(q);
}
                                                                                
void
enqueue(queue* q, node_t* n)
{
    *(q->tail++) = n;
    if (q->tail >= q->limit) q->tail = q->store;
}
                                                                                
node_t *
dequeue(queue* q)
{
    node_t  *n;
    if (q->head == q->tail) n = NULL;
    else {
        n = *(q->head++);
        if (q->head >= q->limit) q->head = q->store;
    }
    return n;
}
