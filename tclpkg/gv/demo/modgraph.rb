#!/usr/bin/ruby

# display the kernel module dependencies

#author: Patricio Ros <patricioros.dev@gmail.com>
# based on: modgraph.tcl by  John Ellson <ellson@research.att.com>

require 'gv'

G = Gv.digraph("G")
N = Gv.protonode(G)
E = Gv.protoedge(G)

Gv.setv(G, 'rankdir', 'LR')
Gv.setv(G, 'nodesep', '0.05')
Gv.setv(N, 'shape', 'box')
Gv.setv(N, 'width', '0')
Gv.setv(N, 'height', '0')
Gv.setv(N, 'margin', '.03')
Gv.setv(N, 'fontsize', '8')
Gv.setv(N, 'fontname', 'helvetica')
Gv.setv(E, 'arrowsize', '.4')

File.readlines("/proc/modules").each do |rec|
  fields = rec.split(' ') 
  n = Gv.node(G, fields[0])
  fields[3].split(',').find_all {|i| not ['-',' '].include? i}.each do |usedby|
     Gv.edge(n, Gv.node(G, usedby))
  end
end

Gv.layout(G, 'dot')
Gv.render(G, 'xlib')

