#!/usr/bin/python

# use layout positioning from within script

import sys
sys.path.append('/usr/lib64/graphviz/python')
sys.path.append('/usr/lib/graphviz/python')
import gv

# create a new empty graph 
G = gv.digraph('G')

# define a simple graph ( A->B )
gv.edge(gv.node(G, 'A'),gv.node(G, 'B'))

# compute a directed graph layout
gv.layout(G, 'dot')

# annotate the graph with the layout information
gv.render(G)

# do something with the layout
n = gv.firstnode(G)
while gv.ok(n) :
    print 'node '+gv.nameof(n)+' is at '+gv.getv(n,'pos')
    e = gv.firstout(n)
    while gv.ok(e) :
	print 'edge '+gv.nameof(gv.tailof(e))+'->'+gv.nameof(gv.headof(e))+' is at '+gv.getv(e,'pos')
	e = gv.nextout(n,e)
    n = gv.nextnode(G,n)
