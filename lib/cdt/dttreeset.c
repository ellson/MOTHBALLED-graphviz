#include	"dthdr.h"

/* Set attributes of a tree.
**
** Written by Kiem-Phong Vo (09/17/2001)
*/

static Dtlink_t* treebalance(Dtlink_t* list, int size)
{
	int		n;
	Dtlink_t	*l, *mid;

	if(size <= 2)
		return list;

	for(l = list, n = size/2 - 1; n > 0; n -= 1)
		l = l->right;

	mid = l->right; l->right = NIL(Dtlink_t*);
	mid->left  = treebalance(list, (n = size/2) );
	mid->right = treebalance(mid->right, size - (n + 1));
	return mid;
}

int dttreeset(Dt_t* dt, int minp, int balance)
{
	int	size;

	if(dt->meth->type != DT_OSET)
		return -1;

	size = dtsize(dt);

	if(minp < 0)
	{	for(minp = 0; minp < DT_MINP; ++minp)
			if((1 << minp) >= size)
				break;
		if(minp <= DT_MINP-4)	/* use log(size) + 4 */
			minp += 4;
	}

	if((dt->data->minp = minp + (minp%2)) > DT_MINP)
		dt->data->minp = DT_MINP;

	if(balance)
		dt->data->here = treebalance(dtflatten(dt), size);

	return 0;
}
