#!/bin/sh
# next line is a comment in tcl \
exec tclsh "$0" ${1+"$@"}

package require Gdtclft

set font /usr/add-on/share/ttf/times.ttf 

set gd [gd create 600 900]
set white [gd color new $gd 255 255 255]
set black [gd color new $gd 0 0 0]

# assemble a string that uses all 256 character codes, on 8 x 32 character lines
set string ""
for {set i 0} {$i < 1024} {incr i} {
	if {! ($i % 32)} {
		if {$i} {
			set string "$string\r\n[format {%-5s: } $i]"
		} {
			set string [format {%-5s: } $i]
		}
	}
	set string "$string[format {%c} $i]"
}

gd text $gd $black $font 14 0 50 50 $string
	
set f [open "| xv -" w]
gd writePNG $gd $f
close $f
