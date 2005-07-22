#!/usr/bin/tclsh

load libgv_tcl.so gv

set g [gv::digraph G]
puts [gv::set $g aaa xxx]
puts [gv::set $g aaa]
set sg [gv::graph $g SG]
set n [gv::node $g hello]
puts [gv::set $n label]
puts [gv::set $n aaa xxx]
puts [gv::set $n aaa]
set m [gv::node $g world]
puts [gv::set $m aaa]
set e [gv::edge $n $m]
puts [gv::set $e aaa xxx]
puts [gv::set $e aaa]
gv::rm $e
gv::rm $n
gv::rm $m
gv::rm $g

set g [gv::stringgraph "digraph G {a->b}"]
gv::rm $g

set g [gv::readgraph ../hello.dot]
gv::layout $g dot
gv::writegraph $g hello.png png
