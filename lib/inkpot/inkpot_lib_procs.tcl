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
            puts -nonewline $f $indent$s[format "%.[set w]s" "        "]
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
proc map {X Y x y r} {
    upvar MAP_[set X][set Y] MAP_XY
    lappend MAP_XY([list $x $y]) $r
}

#crunch the map
proc mapc {X Y} {
    set M MAP_[set X][set Y]
    upvar [set M]          MAP_XY
    upvar [set M]_2[set X] MAP_2X
    upvar [set M]_2[set Y] MAP_2Y
    upvar [set M]_[set X]2 MAP_X2
    upvar [set M]_[set Y]2 MAP_Y2
    #obtain sorted r_sets and use them as keys to the map
    foreach {xy} [array names MAP_XY] {
        foreach {x y} $xy {break}
        set r_set [lsort -ascii $MAP_XY($xy)]
        # set up for finding X and Y from r_sets    
        #using arrays removes duplicate r_sets
        lappend MAP_2X([list $y $r_set]) $x
        lappend MAP_2Y([list $x $r_set]) $y
    }
    #set up for finding maps from X
    foreach {m} [lsort -ascii [array names MAP_2X]] {
        foreach {x} $MAP_2X($m) {
            lappend MAP_X2($x) $m
        }
    }
    #set up for finding maps from Y
    foreach {m} [lsort -ascii [array names MAP_2Y]] {
        foreach {y} $MAP_2Y($m) {
            lappend MAP_Y2($y) $m
        }
    }
if {0} {
    foreach {up map} [list [set M]          MAP_XY \
    			   [set M]_2[set X] MAP_2X \
    			   [set M]_2[set Y] MAP_2Y \
    			   [set M]_[set X]2 MAP_X2 \
    			   [set M]_[set Y]2 MAP_Y2 ] {
	
        puts "size $up = [llength [array names $map]]"
    }
}
}


#list all x
proc map1 {X Y} {
    upvar MAP_[set X][set Y]_[set X]2 MAP_X2
    array names MAP_X2
}
#list all y
proc map2 {X Y} {
    upvar MAP_[set X][set Y]_[set Y]2 MAP_Y2
    array names MAP_Y2
}
#list the r_sets for X
proc map1r {X Y x} {
    upvar MAP_[set X][set Y]_[set X]2 MAP_X2
    set MAP_X2($x)
}
#list the r_sets for Y
proc map2r {X Y y} {
    upvar MAP_[set X][set Y]_[set Y]2 MAP_Y2
    set MAP_Y2($y)
}
#use the map to go from x to {y}'s
proc map12 {X Y x} {
    upvar MAP_[set X][set Y]_[set X]2 MAP_X2
    upvar MAP_[set X][set Y]_2[set Y] MAP_2Y
    set res [list]
    foreach {m} $MAP_X2($x) {
	foreach {y r_set} $m {break}
        lappend res $y
    }
    set res
}
#use the map to go from y to {x}'s
proc map21 {X Y y} {
    upvar MAP_[set X][set Y]_[set Y]2 MAP_Y2
    upvar MAP_[set X][set Y]_2[set X] MAP_2X
    set res [list]
    foreach {m} $MAP_Y2($y) {
	foreach {x r_set} $m {break}
        lappend res $x
    }
    set res
}
#use the map to go from x to {y{r_set}}'s
proc map12r {X Y x} {
    upvar MAP_[set X][set Y]_[set X]2 MAP_X2
    set MAP_X2($x)
}
#use the map to go from y to {x{r_set}}'s
proc map21r {X Y y} {
    upvar MAP_[set X][set Y]_[set Y]2 MAP_Y2
    set MAP_Y2($y)
}
