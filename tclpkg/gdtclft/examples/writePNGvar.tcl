#!/bin/sh
# next line is a comment in tcl \
exec tclsh "$0" ${1+"$@"}

package require Gdtclft

set gd [gd create 200 200]
set white [gd color new $gd 255 255 255]
set black [gd color new $gd 0 0 0]
gd fillpolygon $gd $black 0 0 0 200 200 0
gd writePNGvar $gd foobar

fconfigure stdout -encoding binary
puts $foobar 
