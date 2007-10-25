typedef Agsubnode_t	Agnoderef_t;
typedef Dtlink_t	Agedgeref_t;

#define FIRSTNREF(g)	(agflatten(g), (Agnoderef_t*)(dtfirst((g)->n_seq)))
#define NEXTNREF(rep)  	((Agnoderef_t*)((rep)->seq_link.right?(rep)->seq_link.right - offsetof(Agsubnode_t,seq_link):0))
#define PREVNREF(rep)  	((rep)->seq_link.left)  FIXME
#define NODEOF(rep)		((rep)->node)

#define FIRSTOUTREF(g,sn)	(agflatten(g), (sn)->out_seq)
#define FIRSTINREF(g,sn)	(agflatten(g), (sn)->in_seq)
#define NEXTEREF(sn,rep)  	((rep)->right)
#define PREVEREF(sn,rep)  	((rep)->left)
/* this is expedient but a bit slimey because it "knows" that dict entries of both nodes
and edges are embedded in main graph objects but allocated separately in subgraphs */
#define AGSNMAIN(sn)        ((sn)==(&((sn)->node->mainsub)))
#define EDGEOF(sn,rep)		(AGSNMAIN(sn)?((Agedge_t*)((rep) - offsetof(Agedge_t,seq_link))) : ((Dthold_t*)(rep))->obj)
