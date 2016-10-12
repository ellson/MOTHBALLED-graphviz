/* vim:set shiftwidth=4 ts=4: */

#include <spinehdr.h>
#include <union_find.h>
#include <assert.h>

typedef Agnode_t node_t;

/* union-find */
node_t *UF_find(node_t * n)
{
	while (ND_UF_parent(n) && (ND_UF_parent(n) != n)) {
		if (ND_UF_parent(ND_UF_parent(n)))
			ND_UF_parent(n) = ND_UF_parent(ND_UF_parent(n));
		n = ND_UF_parent(n);
	}
	return n;
}

node_t *UF_union(node_t * u, node_t * v)
{
	if (u == v)
		return u;
	if (ND_UF_parent(u) == NULL) {
		ND_UF_parent(u) = u;
		ND_UF_size(u) = 1;
	} else
		u = UF_find(u);
	if (ND_UF_parent(v) == NULL) {
		ND_UF_parent(v) = v;
		ND_UF_size(v) = 1;
	} else
		v = UF_find(v);
	if (u == v)
		return u;
	if (ND_UF_size(u) < ND_UF_size(v)) {
		ND_UF_parent(u) = v;
		ND_UF_size(v) += ND_UF_size(u);
	} else {
		ND_UF_parent(v) = u;
		ND_UF_size(u) += ND_UF_size(v);
		v = u;
	}
	return v;
}

void UF_remove(node_t * u, node_t * v)
{
	assert(ND_UF_size(u) == 1);
	ND_UF_parent(u) = u;
	ND_UF_size(v) -= ND_UF_size(u);
}

void UF_singleton(node_t * u)
{
	ND_UF_size(u) = 1;
	ND_UF_parent(u) = NULL;
}

void UF_setname(node_t * u, node_t * v)
{
	assert(u == UF_find(u));
	ND_UF_parent(u) = v;
	ND_UF_size(v) += ND_UF_size(u);
}
