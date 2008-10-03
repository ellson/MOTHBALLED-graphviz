#!/usr/bin/tclsh

set fn $argv

set f [open $fn r]
set d [read $f [file size $fn]]
close $f

foreach {. c r g b} [regexp -all -inline -- {\{ \"([^\"]+)\",\s+(\d+),\s+(\d+),\s+(\d+)\s+\},} $d] {
	regsub -all -- { } $c {} c
	set c [string tolower $c]
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

set COLORS() [list tk rgb]

set f [open inkpot_lib_tk.dat w]
puts $f [array get COLORS]
close $f
