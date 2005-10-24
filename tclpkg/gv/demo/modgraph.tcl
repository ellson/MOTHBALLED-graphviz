#!/usr/bin/tclsh

# display the kernel module dependencies 

# author: John Ellson <ellson@research.att.com>

#package require gv

load /usr/lib/graphviz/tcl/gv.so
#load /usr/lib64/graphviz/tcl/gv.so
#load $env(PREFIX)/lib/graphviz/tcl/gv.so

set f [open /proc/modules r]
set modules [read -nonewline $f]
close $f

set g [gv::digraph G]
gv::setv $g rankdir LR
gv::setv $g nodesep .05

gv::setv $g node shape box
gv::setv $g node width 0
gv::setv $g node height 0
gv::setv $g node margin .03
gv::setv $g node fontsize 8
gv::setv $g node fontname helvetica

gv::setv $g edge arrowsize .4

foreach rec [split $modules \n] {
  set n [gv::node $g [lindex $rec 0]]
  foreach usedby [split [lindex $rec 3] ,] {
    if {[string equal $usedby -] || [string equal $usedby {}]} {continue}
    set e [gv::edge $n [gv::node $g $usedby]]
  }
}

gv::layout $g dot
gv::render $g gtk
