#!/usr/bin/tclsh

set fn $argv

set f [open $fn r]
set d [read $f [file size $fn]]
close $f
regsub -all -- {[-= \t\n\r\"/]} $d {} d

foreach {. c rgb r g b} [regexp -all -inline -- {e>(\w+)<\w+><\w+><\w+><\w+>(\w+)\((\d+),(\d+),(\d+)\)} $d] {
	if {![string equal $rgb rgb]} {
		puts stderr "non rgb color found: $c $rgb $r $g $b"
	}
	if {[info exists COLORS($c)]} {
                foreach {rr gg bb} $COLORS($c) {break}
                if {$r != $rr || $g != $gg || $b != $bb} {
			puts "$c is not unique and rgb doesn't match: $r $g $b != $rr $gg $bb"
		} {
#                   puts "$c is not unique"     
		}
	}
	set COLORS($c) [list $r $g $b]
}

set COLORS() [list svg rgb]

set f [open inkpot_lib_svg.dat w]
puts $f [array get COLORS]
close $f
