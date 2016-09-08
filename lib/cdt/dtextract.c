#include	"dthdr.h"

/*	Extract objects of a dictionary.
**
**	Written by Kiem-Phong Vo (5/25/96).
*/

Dtlink_t* dtextract(reg Dt_t* dt)
{
	reg Dtlink_t	*list, **s, **ends;

	if(dt->data->type&(DT_OSET|DT_OBAG) )
		list = dt->data->here;
	else if(dt->data->type&(DT_SET|DT_BAG))
	{	list = dtflatten(dt);
		for(ends = (s = dt->data->htab) + dt->data->ntab; s < ends; ++s)
			*s = NIL(Dtlink_t*);
	}
	else /*if(dt->data->type&(DT_LIST|DT_STACK|DT_QUEUE))*/
	{	list = dt->data->head;
		dt->data->head = NIL(Dtlink_t*);
	}

	dt->data->type &= ~DT_FLATTEN;
	dt->data->size = 0;
	dt->data->here = NIL(Dtlink_t*);

	return list;
}
