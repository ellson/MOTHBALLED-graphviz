#!/bin/sh
# next line is a comment in tcl \
exec tclsh "$0" ${1+"$@"}

package require Gdtclft

set font /usr/local/share/ttf/mingliu.ttc

set string "ABC,abc.GJQYZ?gjqyz!M"

set gd [gd create 600 700]
set white [gd color new $gd 255 255 255]
set green [gd color new $gd 0 255 0]
set black [gd color new $gd 0 0 0]

set size 20
set angle 0
set x 50
set y 60
gd text $gd $black $font $size $angle $x $y $string

set f [open "| xv -" w]
#set f [open "test.png" w]
gd writePNG $gd $f
close $f
