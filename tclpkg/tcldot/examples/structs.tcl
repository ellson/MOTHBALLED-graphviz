#!/bin/sh
# next line is a comment in tcl \
exec wish "$0" ${1+"$@"}

package require Tkspline
package require Tcldot

######################################################################
# first lets see what it looks like from a gv-syntax input:
set s {
	digraph structs {
	node [shape=record];
    	struct1 [shape=record,label="<f0> left|<f1> middle|<f2> right"];
    	struct2 [shape=record,label="<f0> one|<f1> two"];
    	struct3 [shape=record,label="hello\nworld |{ b |{c|<here> d|e}| f}| g | h"];
    	struct1:f1 -> struct2:f0;
    	struct1:f2 -> struct3:here;
	}
}
set g [dotstring $s]
eval [$g render [canvas .c]]
pack .c


######################################################################
# take a look at what is in the graph
proc putsobj {obj} {
	puts "[format {%-7s} $obj] [$obj showname]"
	foreach a [$obj listattributes] {
		puts "    $a=[$obj queryattribute $a]"
	}
}

foreach n [$g listnodes] {
	putsobj $n
}
foreach n [$g listnodes] {
	foreach e [$n listoutedges] {
		putsobj $e
	}
}

######################################################################
# show how the same effect can be achieved from tcl input


set structs [dotnew digraph structs]
$structs setnodeattributes shape record
$structs addnode struct1 label "<f0> left|<f1> middle|<f2> right"
$structs addnode struct2 label "<f0> one|<f1> two"
$structs addnode struct3 label "hello\nworld |{ b |{c|<here> d|e}| f}| g | h"
$structs addedge struct1 struct2 tailport :f1 headport :f0
$structs addedge struct1 struct3 tailport :f2 headport :here

toplevel .t
eval [$structs render [canvas .t.c]]
pack .t.c
