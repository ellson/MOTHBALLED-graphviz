#!/usr/bin/tclsh

load gv.so

set g [gv::digraph G]
puts $g
puts [gv::set $g aaa xxx]
puts [gv::set $g aaa]
set sg [gv::graph $g SG]
puts $sg
set n [gv::node $g hello]
puts $n
puts [gv::set $n label]
puts [gv::set $n aaa xxx]
puts [gv::set $n aaa]
set m [gv::node $g world]
puts $m
puts [gv::set $m aaa]
set e [gv::edge $n $m]
puts $e
puts [gv::set $e aaa xxx]
puts [gv::set $e aaa]
gv::rm $e
gv::rm $n
gv::rm $m
gv::rm $g

set g [gv::stringgraph "digraph G {a->b}"]
puts $g
gv::rm $g

set g [gv::readgraph hello.dot]
puts $g
gv::layout $g dot
gv::writegraph $g hello_tcl.png png
gv::rm $g
