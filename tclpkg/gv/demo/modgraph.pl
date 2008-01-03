#!/usr/bin/perl

# display the kernel module dependencies

# author: John Ellson <ellson@research.att.com>

use gv;

$G = gv::digraph("G");
$N = gv::protonode($G);
$E = gv::protoedge($G);

gv::setv($G, "rankdir", "LR");
gv::setv($G, "nodesep", "0.05");
gv::setv($N, "shape", "box");
gv::setv($N, "width", "0");
gv::setv($N, "height", "0");
gv::setv($N, "margin", ".03");
gv::setv($N, "fontsize", "8");
gv::setv($N, "fontname", "helvetica");
gv::setv($E, "arrowsize", ".4");

#FIXME - complete translation to perl

#f = File.open('/proc/modules', mode="r")
#while ! f.eof do
#	rec = f.gets()
#
#    for mod, usedbylist in string.gfind(rec, "([_%w]+) %w+ %w+ ([-,_%w]+)") do
#       n = gv.node(G, mod)
#       for usedby in string.gfind(usedbylist, "([-_%w]+)") do
#          if (usedby ~= '-') and (usedby ~= '') then
#             gv.edge(n, gv.node(G, usedby))
#          end
#       end
#    end
#
#end	
#f.close

gv::layout($G, "dot");
gv::render($G, "xlib");
