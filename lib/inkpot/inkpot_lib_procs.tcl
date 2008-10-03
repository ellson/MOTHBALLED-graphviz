# some support procs

###########################################################
# TAB  support formatting of outputput tables

proc tab_begin {f s} {
	upvar pos pos

	puts $f $s
	set pos 0
}

proc tab_end {f s} {
	upvar pos pos

	if {$pos} { puts $f "" }
	puts $f $s
	set pos 0
}

# $comment_list needs to be 8 char or less
proc tab_begin_block {f {comment_list {}}} {
	upvar pos pos
	upvar comments comments
	upvar indent indent

	if {$pos == 0} {
		incr pos [string length $indent]
		if {$comments && [llength $comment_list]} {
			set s [concat "/*" $comment_list "*/"]
			incr pos [string length $s]
			set w [expr 16 - $pos]
                	puts -nonewline $f $indent$s[format "%.[set w]s" "                "]
			set pos 16
        	} {
			puts -nonewline $f $indent
		}
	}
}

proc tab_end_block {f {comment_list {}}} {
	upvar pos pos
	upvar comments comments
	upvar target_line_length target_line_length
	
	if {$comments && [llength $comment_list]} {
		set w [expr 5 - $pos / 8]
		if {$w < 0} {set w 0}
		set s [concat "/*" $comment_list "*/"]
		puts $f [format "%.[set w]s" "\t\t\t\t\t"]$s
		set pos 0
	} {
		if {$pos >= $target_line_length} {
			puts $f ""
			set pos 0
		}
	}
}

proc tab_elem {f s} {
	upvar pos pos

	puts -nonewline $f $s
	incr pos [string length $s]
}

###################################################################
# DEBUG

# print some summary information about an array
proc print_first {a} {
	upvar $a b
	set size [llength [array names b]]
	set first [lindex [array names b] 0]
	set value $b($first)
	set totelem 0
	set maxelem 0
	foreach {n} [array names b] {
		set elems [llength $b($n)]
		if {$elems > $maxelem} {set maxelem $elems}
		incr totelem $elems
	}
	puts stderr [list $a size:$size maxelem:$maxelem totelem:$totelem first: $a\($first) $value]
}

####################################################################
#  MAP  --mapping m:n relationships between elemenets of 2 sets

#populate the map
proc mapw {XY x y r} {
	upvar MAP_[set XY] MAP_XY
	lappend MAP_XY([list $x $y]) $r
}

#crunch the map
proc maps {XY} {
	foreach {X Y} [split $XY {}] {break}
	upvar MAP_[set XY] MAP_XY
	upvar MAP_2[set X] MAP_2X
	upvar MAP_2[set Y] MAP_2Y
	upvar MAP_[set X]2 MAP_X2
	upvar MAP_[set Y]2 MAP_Y2
	#obtain sorted r_sets and use them as keys to the map
	foreach {xy} [array names MAP_XY] {
		foreach {x y} $xy {break}
		set r_set [lsort -ascii $MAP_XY($xy)]
		# set up for finding X and Y from r_sets	
		#using arrays removes duplicate r_sets
		lappend MAP_2X($r_set) $x
		lappend MAP_2Y($r_set) $y
	}
	#set up for finding rsets from X
	set MAP_X2() $Y
	foreach {r_set} [lsort -ascii [array names MAP_2X]] {
		foreach {x} $MAP_2X($r_set) {
			lappend MAP_X2($x) $r_set
		}
	}
	#set up for finding rsets from Y
	set MAP_Y2() $X
	foreach {r_set} [lsort -ascii [array names MAP_2Y]] {
		foreach {y} $MAP_2Y($r_set) {
			lappend MAP_Y2($y) $r_set
		}
	}
if {1} {
	foreach {map up} [list  MAP_XY [set XY]
				MAP_2X 2[set X]
				MAP_2Y 2[set Y] 
				MAP_X2 [set X]2
				MAP_Y2 [set Y]2] {
		puts "size MAP_$up = [llength [array names $map]]"
	}
}
}

#use the map
proc mapr {K {k {}}} {
	upvar MAP_[set K]2 MAP_K2
	set J $MAP_K2()
	upvar MAP_2[set J] MAP_2J
	set res [list]
	foreach {r_set} $MAP_K2($k) {
		lappend res $MAP_2J($r_set)
	}
	set res
}
