#!/usr/bin/tclsh

set fn $argv

set f [open $fn r]
set d [read $f [file size $fn]]
close $f

regsub -all -line -- {#.*$} $d {} d
regsub -all -line -- {$} $d {:} d
regsub -all -line -- {^} $d {:} d
regsub -all -- {\"} $d {} d

foreach {rec ColorName NumOfColors Type CritVal ix ixa r g b SchemeType} [regexp -all -inline -- {:(\w*),(\d*),(\w*),([.\d]*),(\d+),(\w+),(\d+),(\d+),(\d+),(\w*):} $d] {

	if {[string length $ColorName]} {
		set nam $ColorName
		set n $NumOfColors
		set typ $Type
		set crit $CritVal
		if {[string length $SchemeType]} {
			set styp $SchemeType
		}
		
		set t $nam
		set l [string length $t]
		regsub -all -- {\d} $d {} d
		set ll [string length $t]
        	if {$l != $ll} {
			puts stderr "$argv0: scheme $nam contains digits!"
		}
	}

	if {! [info exists nam]} {
		puts stderr "$argv0: ColorName was not set before use"
	}

if {1} {
#
# oranges/j is inconsistent
#
	set ixn [string first $ixa "abcdefghijklmnopqrstuvwxyz"]
	if {$ixn < 0} {
		puts stderr "$argv0: Failed to convert alpha index \"$ixa\" to an integer"
	}
	incr ixn

	set nam_ixn [list $nam $ixn]
        if {[info exists COLORS($nam_ixn)]} {
                foreach {rr gg bb} $COLORS($nam_ixn) {break}
        	if {$r != $rr || $g != $gg || $b != $bb} {
			puts stderr ""
			puts stderr "$argv0: \"$nam $ixa\" is not unique"
			puts stderr " and rgb doesn't match: $r $g $b != $rr $gg $bb"
			puts stderr " in record: [string trim $rec :]"
			puts stderr " Retaining first value found, discarding second."
			puts stderr ""
		} {
			#                   puts "$nam ixa is not unique"     
		}
	}
	set COLORS($nam_ixn) [list $r $g $b]
}

if {1} {
	set nam_n_ix [list $nam $n $ix]
        if {[info exists COLORS($nam_n_ix)]} {
                foreach {rr gg bb} $COLORS($nam_n_ix) {break}
        	if {$r != $rr || $g != $gg || $b != $bb} {
			puts stderr "\"$nam_n_ix\" is not unique and rgb doesn't match: $r $g $b != $rr $gg $bb"
			puts stderr $rec
			puts ":$nam,$n,$typ,$crit,$ix,$ixa,$r,$g,$b,$styp:"
			puts "\"$ixa\" = $ixn"

			puts stderr ""
			foreach {color} [lrange [lsort -ascii [array name COLORS]] end-5 end] {
				puts stderr "$argv0: COLORS\($color\) = $COLORS($color)"
			}
			exit
		} {
			#                   puts "$nam_n_ix is not unique"     
		}
	}
	set COLORS($nam_n_ix) [list $r $g $b]
}
}

set COLORS() [list brewer rgb]

set f [open inkpot_lib_brewer.dat w]
puts $f [array get COLORS]
close $f
