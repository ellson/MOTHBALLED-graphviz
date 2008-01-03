#!/usr/bin/tclsh

# display the kernel module dependencies 

# author: John Ellson <ellson@research.att.com>

package require gv

set f [open /proc/modules r]
set modules [read -nonewline $f]
close $f

set G [gv::digraph G]
set N [gv::protonode $G]
set E [gv::protoedge $G]

gv::setv $G rankdir LR
gv::setv $G nodesep .05
gv::setv $N shape box
gv::setv $N width 0
gv::setv $N height 0
gv::setv $N margin .03
gv::setv $N fontsize 8
gv::setv $N fontname helvetica
gv::setv $E arrowsize .4

foreach rec [split $modules \n] {
  set n [gv::node $G [lindex $rec 0]]
  foreach usedby [split [lindex $rec 3] ,] {
    if {[string equal $usedby -] || [string equal $usedby {}]} {continue}
    set e [gv::edge $n [gv::node $G $usedby]]
  }
}

gv::layout $G dot
gv::render $G xlib
