#!/usr/bin/python

# use layout positioning from within script

import sys
sys.path.append('/usr/lib64/graphviz/python')
import gv

# create a new empty graph 
G = gv.digraph('G')

# define a simple graph ( A->B )
gv.edge(gv.node(G, 'A'),gv.node(G, 'B'))

# compute a directed graph layout
gv.layout(G, 'dot')

# annotate the graph with the layout information
gv.render(G, 'ps')

