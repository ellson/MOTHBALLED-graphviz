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

set g [gv::readstring "digraph G {a->b}"]
gv::rm $g

set g [gv::read hello.dot]
for {set n [gv::firstnode $g]} {[gv::ok $n]} {set n [gv::nextnode $g $n]} {
    puts [gv::nameof $n]
    for {set a [gv::firstattr $n]} {[gv::ok $a]} {set a [gv::nextattr $n $a]} {
        puts "    [gv::nameof $a] [gv::set $n $a]"
    }
}
for {set n [gv::firstnode $g]} {[gv::ok $n]} {set n [gv::nextnode $g $n]} {
    for {set e [gv::firstout $n]} {[gv::ok $e]} {set e [gv::nextout $n $e]} {
	puts "[gv::nameof $n] -> [gv::nameof [gv::headof $e]]"
	for {set a [gv::firstattr $e]} {[gv::ok $a]} {set a [gv::nextattr $e $a]} {
	    puts "    [gv::nameof $a] [gv::set $e $a]"
	}
    }
}

gv::layout $g dot
gv::render $g png hello.png
