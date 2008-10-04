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
#  MAP  --mapping m:n relationships between elements of 2 sets

#populate the map X<=>Y with a single x-y pairing labeled r
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
    upvar [set M]_[set X]a MAP_Xa
    upvar [set M]_[set Y]a MAP_Ya

    array unset MAP_2X
    array unset MAP_2Y
    array unset MAP_X2
    array unset MAP_Y2
    array unset MAP_Xa
    array unset MAP_Ya

    #obtain sorted r_set's and use them as keys to the map
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
    #set up for finding aliases - all x that share the same map to the same set of y's
    foreach {y} [lsort -ascii [array names MAP_Y2]] {
	lappend MAP_Xa($MAP_Y2($y)) $y
    }
    #set up for finding aliases - all y that share the same map to the same set of x's
    foreach {x} [lsort -ascii [array names MAP_X2]] {
	lappend MAP_Ya($MAP_X2($x)) $x
    }

if {0} {
    foreach {up map} [list [set M]          MAP_XY \
    			   [set M]_2[set X] MAP_2X \
    			   [set M]_2[set Y] MAP_2Y \
    			   [set M]_[set X]2 MAP_X2 \
    			   [set M]_[set Y]2 MAP_Y2 \
			   [set M]_[set X]a MAP_Xa \
			   [set M]_[set Y]a MAP_Ya ] {
        print_first $map
    }
}
}


#list all x
proc map1 {X Y} {
    upvar MAP_[set X][set Y]_[set X]2 MAP_X2
    lsort [array names MAP_X2]
}
#list all y
proc map2 {X Y} {
    upvar MAP_[set X][set Y]_[set Y]2 MAP_Y2
    lsort [array names MAP_Y2]
}
#list the map for x, result of the form: {y {r...}}
proc map1m {X Y x} {
    upvar MAP_[set X][set Y]_[set X]2 MAP_X2
    set MAP_X2($x)
}
#list the map for y, result of the form: {x {r...}}
proc map2m {X Y y} {
    upvar MAP_[set X][set Y]_[set Y]2 MAP_Y2
    set MAP_Y2($y)
}
#list all x->y maps, result of the form: {{y {r...}}...}
proc map1ms {X Y} {
    upvar MAP_[set X][set Y]_2[set Y] MAP_2Y
    lsort [array names MAP_2Y]
}
#list all y->x maps, result of the form: {{x {r...}}...}
proc map2ms {X Y} {
    upvar MAP_[set X][set Y]_2[set X] MAP_2X
    lsort [array names MAP_2X]
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
    lsort $res
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
    lsort $res
}
# find aliases - go from map: {x {r...}, to the list of x that share the same mapping
proc map1ma {X Y m} {
    upvar MAP_[set X][set Y]_[set Y]a MAP_Ya
    lsort $MAP_Ya($m)
}
# find aliases - go from map: {y {r...}, to the list of y that share the same mapping
proc map2ma {X Y m} {
    upvar MAP_[set X][set Y]_[set X]a MAP_Xa
    lsort $MAP_Xa($m)
}
# find aliases sets for x
proc map1mas {X Y} {
    upvar MAP_[set X][set Y]_[set Y]a MAP_Ya
    lsort [array names MAP_Ya]
}
# find aliases sets for y
proc map2mas {X Y} {
    upvar MAP_[set X][set Y]_[set X]a MAP_Xa
    lsort [array names MAP_Xa]
}


if {0} {  ;# for debug
	map C V black 0 svg
	map C V black 0 x11
	map C V black 0 tk
	map C V grey0 0 svg
	map C V grey0 0 x11
	map C V grey0 0 tk
	map C V noir 0 french
	map C V green 2 svg
	map C V lime 1 svg
	map C V green 1 x11
	map C V green 1 tk
	map C V lime  1 tk
	map C V lime  1 x11
	map C V vert 1 french

	mapc C V

	puts "map1 C V        : [map1 C V]"
	puts "       expected : black green grey0 lime noir vert"
	puts "map2 C V        : [map2 C V]"
       	puts "       expected : 0 1 2"
	puts "map1m C V green : [map1m C V green]"
	puts "       expected : {1 {tk x11}} {2 svg}"
	puts "map2m C V 2     : [map2m C V 2]"
	puts "       expected : {green svg}"
	puts "map1ms C V      : [map1ms C V]"
	puts "       expected : {black {svg tk x11}} {green svg} {green {tk x11}} {grey0 {svg tk x11}} {lime {svg tk x11}} {noir french} {vert french}"
	puts "map2ms C V      : [map2ms C V]"
       	puts "       expected : {0 french} {0 {svg tk x11}} {1 french} {1 {svg tk x11}} {1 {tk x11}} {2 svg}"
	puts "map12 C V green : [map12 C V green]"
	puts "       expected : 1 2"
	puts "map21 C V 1     : [map21 C V 1]"
	puts "       expected : green lime vert"
	puts "map1ma C V [map1m C V black] : [map1ma C V [map1m C V black]]"
	puts "       expected : black grey0"
	puts "map2ma C V [map2m C V 1] : [map2ma C V [map2m C V 1]]"
	puts "       expected : 1"
}
