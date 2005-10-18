#!/usr/bin/python

# display the kernel module dependencies

# author: Michael Hohn <mhholn@lbl.gov>
#  based on: modgraph.tcl by John Ellson <ellson@research.att.com>

import sys
# sys.path.append('/usr/lib/graphviz/python')
sys.path.append('/usr/lib64/graphviz/python')
import gv

modules = open("/proc/modules", 'r').readlines()

G = gv.digraph("G")
gv.setv(G, 'rankdir', 'LR')
gv.setv(G, 'nodesep', '0.05')
gv.setv(G, 'node', 'shape', 'box')
gv.setv(G, 'node', 'width', '0')
gv.setv(G, 'node', 'height', '0')
gv.setv(G, 'node', 'margin', '.03')
gv.setv(G, 'node', 'fontsize', '8')
gv.setv(G, 'node', 'fontname', 'helvetica')
gv.setv(G, 'edge', 'arrowsize', '.4')

for rec in modules:
   fields = rec.split(' ')
   n = gv.node(G, fields[0])
   for usedby in fields[3].split(','):
      if (usedby != '-') & (usedby != ''):
         gv.edge(n, gv.node(G, usedby))

gv.layout(G, 'dot')
# The 'xlib' renderer is provided by graphviz-cairo
gv.render(G, 'xlib')
