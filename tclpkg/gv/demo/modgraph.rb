#!/usr/bin/ruby

# display the kernel module dependencies

# author: John Ellson <ellson@research.att.com>

require 'gv'

G = Gv.digraph("G")
Gv.setv(G, 'rankdir', 'LR')
Gv.setv(G, 'nodesep', '0.05')
Gv.setv(G, 'node', 'shape', 'box')
Gv.setv(G, 'node', 'width', '0')
Gv.setv(G, 'node', 'height', '0')
Gv.setv(G, 'node', 'margin', '.03')
Gv.setv(G, 'node', 'fontsize', '8')
Gv.setv(G, 'node', 'fontname', 'helvetica')
Gv.setv(G, 'edge', 'arrowsize', '.4')

f = File.open('/proc/modules', mode="r")
while ! f.eof do
	rec = f.gets()

#FIXME - complete translation to ruby

#    for mod, usedbylist in string.gfind(rec, "([_%w]+) %w+ %w+ ([-,_%w]+)") do
#       n = gv.node(G, mod)
#       for usedby in string.gfind(usedbylist, "([-_%w]+)") do
#          if (usedby ~= '-') and (usedby ~= '') then
#             gv.edge(n, gv.node(G, usedby))
#          end
#       end
#    end

end	
f.close

Gv.layout(G, 'dot')
Gv.render(G, 'gtk')
