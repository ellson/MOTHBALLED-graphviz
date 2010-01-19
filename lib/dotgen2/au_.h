int gvgetint(void *obj, char *str, int defval);
int gvround(double arg);

extern queue        *new_queue(int);
extern void     	enqueue(queue *, Agnode_t *);
extern void			enqueue_neighbors(queue *, Agnode_t *, int);
extern Agnode_t		*dequeue(queue *);
extern void			free_queue(queue *);
extern void         *zmalloc(size_t);

extern void			start_timer(void);
extern double		elapsed_sec(void);

void warn3(char *s0, char *s1, char *s2);

int crossings_below(Agraph_t *g, int northlevel);
Agedge_t *agfindedge(Agraph_t *g, Agnode_t *t, Agnode_t *h);
void agautoinit(Agraph_t *g, int, int, int);
