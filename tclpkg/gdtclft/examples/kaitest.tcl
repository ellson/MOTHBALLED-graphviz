#!/bin/sh
# next line is a comment in tcl \
exec tclsh "$0" ${1+"$@"}

package require Gdtclft

set font /usr/local/share/ttf/kai.ttf

set gd [gd create 600 700]
set white [gd color new $gd 255 255 255]
set black [gd color new $gd 0 0 0]

set angle 0
set size 14
set color $black
set x 50

set y 30
#set string "abcdefghijklmnopqrstuvwxyz\r\nABCDEFGHIJKLMNOPQRSTUVWXYZ"
set string "The next line is JIS-8 coded:\r\n翻譯與產品國際化"
gd text $gd $color $font $size $angle $x $y $string

set f [open "| xv -" w]
gd writePNG $gd $f
close $f
