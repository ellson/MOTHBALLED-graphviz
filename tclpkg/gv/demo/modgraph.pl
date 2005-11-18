#!/usr/bin/perl

# display the kernel module dependencies

# author: John Ellson <ellson@research.att.com>

#use lib "/usr/lib/graphviz/perl";
use lib "/home/ellson/FIX/Linux.i686/lib/graphviz/perl";
use gv;

$g = gv::digraph "G";
gv::setv($g, "rankdir", "LR");
gv::setv($g, "nodesep", "0.05");
gv::setv($g, "node", "shape", "box");
gv::setv($g, "node", "width", "0");
gv::setv($g, "node", "height", "0");
gv::setv($g, "node", "margin", ".03");
gv::setv($g, "node", "fontsize", "8");
gv::setv($g, "node", "fontname", "helvetica");
gv::setv($g, "edge", "arrowsize", ".4");

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

gv::layout($g, "dot");
gv::render($g, "gtk");
