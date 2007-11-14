#!/usr/bin/php
<?
#FIXME  - I don't know how to extend php's paths.  Needs:
#   ln -s /usr/lib64/graphviz/php/libgv_php.so /usr/lib64/php/modules/gv.so
#   ln -s /usr/lib64/graphviz/php/php/gv.php /usr/share/php/gv.php

include("gv.php");

# display the kernel module dependencies

# author: John Ellson <ellson@research.att.com>

$G = digraph("G");
$N = protonode($G);
$E = protoedge($G);

setv($G, "rankdir", "LR");
setv($G, "nodesep", "0.05");
setv($N, "shape", "box");
setv($N, "width", "0");
setv($N, "height", "0");
setv($N, "margin", ".03");
setv($N, "fontsize", "8");
setv($N, "fontname", "helvetica");
setv($E, "arrowsize", ".4");

$f = fopen("/proc/modules", "r");
while ( ! feof($f)) {
	$rec = fgets($f);

#    for mod, usedbylist in string.gfind(rec, "([_%w]+) %w+ %w+ ([-,_%w]+)") do
#       n = gv.node(G, mod)
#       for usedby in string.gfind(usedbylist, "([-_%w]+)") do
#          if (usedby ~= '-') and (usedby ~= '') then
#             gv.edge(n, gv.node(G, usedby))
#          end
#       end
#    end
#
}
fclose($f);

#layout($G, "dot");
#render($G, "png");

?>
