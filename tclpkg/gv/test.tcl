#!/usr/bin/tclsh

load [pwd]/libgv_tcl.so gv

set g [gv::digraph G]
puts [gv::setv $g aaa xxx]
puts [gv::getv $g aaa]
set sg [gv::graph $g SG]
set n [gv::node $g hello]
puts [gv::getv $n label]
puts [gv::setv $n aaa xxx]
puts [gv::getv $n aaa]
set m [gv::node $g world]
puts [gv::getv $m aaa]
set e [gv::edge $n $m]
puts [gv::setv $e aaa xxx]
puts [gv::getv $e aaa]
gv::rm $e
gv::rm $n
gv::rm $m
gv::rm $g

set g [gv::readstring "digraph G {a->b}"]
gv::rm $g

set g [gv::read hello.gv]
for {set n [gv::firstnode $g]} {[gv::ok $n]} {set n [gv::nextnode $g $n]} {
    puts [gv::nameof $n]
    for {set a [gv::firstattr $n]} {[gv::ok $a]} {set a [gv::nextattr $n $a]} {
        puts "    [gv::nameof $a] [gv::getv $n $a]"
    }
}
for {set n [gv::firstnode $g]} {[gv::ok $n]} {set n [gv::nextnode $g $n]} {
    for {set e [gv::firstout $n]} {[gv::ok $e]} {set e [gv::nextout $n $e]} {
	puts "[gv::nameof $n] -> [gv::nameof [gv::headof $e]]"
	for {set a [gv::firstattr $e]} {[gv::ok $a]} {set a [gv::nextattr $e $a]} {
	    puts "    [gv::nameof $a] [gv::getv $e $a]"
	}
    }
}

gv::layout $g dot
gv::render $g png hello.png
