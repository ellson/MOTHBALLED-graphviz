#!/usr/bin/lua

-- display the kernel module dependencies

-- author: John Ellson <ellson@research.att.com>

lib = loadlib('/usr/lib/graphviz/lua/libgv_lua.so','Gv_Init')
-- lib = loadlib('/usr/lib64/graphviz/lua/libgv_lua.so','Gv_Init')
assert(lib)()

modules = io.lines('/proc/modules') 

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

for rec in modules do
   for mod, usedbylist in string.gfind(rec, "([_%w]+) %w+ %w+ ([-,_%w]+)") do
      n = gv.node(G, mod)
      for usedby in string.gfind(usedbylist, "([-_%w]+)") do
         if (usedby ~= '-') and (usedby ~= '') then
            gv.edge(n, gv.node(G, usedby))
         end
      end
   end
end

gv.layout(G, 'dot')
gv.render(G, 'gtk')
