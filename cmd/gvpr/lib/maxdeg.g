/* find node of max degree */
BEGIN {node_t n; int maxd = -1; }
N { if (degree > maxd) {maxd = degree; n = $;} }
END {printf ("max degree = %d, node %s\n", maxd, n.name)}
