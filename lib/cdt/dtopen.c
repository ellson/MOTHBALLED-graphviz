#include	"dthdr.h"
static char*     Version = "\n@(#)$Id$\0\n";

/* 	Make a new dictionary
**
**	Written by Kiem-Phong Vo (5/25/96)
*/

Dt_t* dtopen(Dtdisc_t* disc, Dtmethod_t* meth)
{
	Dt_t*		dt = (Dt_t*)Version;	/* shut-up unuse warning */
	reg int		e;
	Dtdata_t*	data;

	if(!disc || !meth)
		return NIL(Dt_t*);

	/* allocate space for dictionary */
	if(!(dt = (Dt_t*) malloc(sizeof(Dt_t))))
		return NIL(Dt_t*);

	/* initialize all absolutely private data */
	dt->searchf = NIL(Dtsearch_f);
	dt->meth = NIL(Dtmethod_t*);
	dt->disc = NIL(Dtdisc_t*);
	dtdisc(dt,disc,0);
	dt->type = DT_MALLOC;
	dt->nview = 0;
	dt->view = dt->walk = NIL(Dt_t*);
	dt->user = NIL(void*);

	if(disc->eventf)
	{	/* if shared/persistent dictionary, get existing data */
		data = NIL(Dtdata_t*);
		if((e = (*disc->eventf)(dt,DT_OPEN,(void*)(&data),disc)) < 0)
			goto err_open;
		else if(e > 0)
		{	if(data)
			{	if(data->type&meth->type)
					goto done;
				else	goto err_open;
			}

			if(!disc->memoryf)
				goto err_open;

			free((void*)dt);
			if(!(dt = (*disc->memoryf)(0, 0, sizeof(Dt_t), disc)) )
				return NIL(Dt_t*);
			dt->searchf = NIL(Dtsearch_f);
			dt->meth = NIL(Dtmethod_t*);
			dt->disc = NIL(Dtdisc_t*);
			dtdisc(dt,disc,0);
			dt->type = DT_MEMORYF;
			dt->nview = 0;
			dt->view = dt->walk = NIL(Dt_t*);
		}
	}

	/* allocate sharable data */
	if(!(data = (Dtdata_t*)(dt->memoryf)(dt,NIL(void*),sizeof(Dtdata_t),disc)) )
	{ err_open:
		free((void*)dt);
		return NIL(Dt_t*);
	}

	data->type = meth->type;
	data->here = NIL(Dtlink_t*);
	data->htab = NIL(Dtlink_t**);
	data->ntab = data->size = data->loop = 0;
	data->minp = 0;

done:
	dt->data = data;
	dt->searchf = meth->searchf;
	dt->meth = meth;

	if(disc->eventf)
		(*disc->eventf)(dt, DT_ENDOPEN, (void*)dt, disc);

	return dt;
}
