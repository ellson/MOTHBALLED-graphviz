# some support procs

###########################################################
# TAB  support formatting of outputput tables

# private procs
proc tab_start_row { } {
    global comments
    upvar 2 TAB_pos         pos
    upvar 2 TAB_chan        ch
    upvar 2 TAB_row_start   rstart
    set indent "  "
    if {$pos == 0} {
        if  {$comments} {
            set s "  /* [uplevel 2 $rstart] */ "
            set w [expr 16 - [string length $s]]
            if {$w < 0} {set w 0}
            set s $indent$s[format "%.[set w]s " "        "]
        } {
	    set s $indent
        }
        puts -nonewline $ch $s
        set pos [string length $s]
    }
}

proc tab_end_row { } {
    global comments
    upvar 2 TAB_pos         pos
    upvar 2 TAB_chan        ch
    upvar 2 TAB_row_end     rend
    if {$comments} {
        set w [expr 5 - $pos / 8]
        if {$w < 0} {set w 0}
        set s " /* [uplevel 2 $rend] */"
        puts $ch [format "%.[set w]s" "\t\t\t\t\t"]$s
        set pos 0
    }
}


# public procs
proc tab_initialize {
    chan
    row_start
    row_end
    table_start
} {
    upvar TAB_pos         pos
    upvar TAB_chan        ch
    upvar TAB_row_start   rstart
    upvar TAB_row_end     rend
    set ch $chan
    set rstart $row_start
    set rend $row_end
    puts $ch $table_start
    set pos 0
}

proc tab_finalize { table_end } {
    upvar TAB_pos         pos
    upvar TAB_chan        ch
    if {$pos} { tab_end_row }
    puts $ch $table_end
}

proc tab_first_elem {data} {
    upvar TAB_pos         pos
    upvar TAB_chan        ch
    if {$pos} { tab_end_row }
    tab_start_row
    puts -nonewline $ch $data
    incr pos [string length $data]
}

proc tab_last_elem {data} {
    upvar TAB_pos         pos
    upvar TAB_chan        ch
    puts -nonewline $ch $data
    incr pos [string length $data]
    tab_end_row
}

proc tab_next_elem {data} {
    upvar TAB_pos         pos
    upvar TAB_chan        ch
    if {$pos > 64} { tab_end_row }
    if {$pos == 0} { tab_start_row }
    puts -nonewline $ch $data
    incr pos [string length $data]
}


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
    upvar #0 $a b
    set size [llength [array names b]]
    if {$size} {
    	set first [lindex [array names b] 0]
    	set value $b($first)
    } {
	set first {}
	set value {}
    }
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
#
# Public procs:
#
# map {M x y r}		: put into M the relation x-y labeled r
# 
# map1 {M}		: get {x...}
# map2 {M}		: get {y...}
# map3 {M}		: get {r...}
# mapm1 {M}		: get {{x {r...}}...}
# mapm2 {M} 		: get {{y {r...}}...}
# map1m2 {M x}		: from x get {y {r...}}
# map2m1 {M y}		: from y get {x {r...}}
# map3m1 {M r_set}	: from r_set get {{x {r...}}...}
# map3m2 {M r_set}	: from r_set get {{y {r...}}...}
# mapm21 {M m2}		: from m2 get {x...}
# mapm12 {M m1}		: from m1 get {y...}
# map12 {M x}		: from x get {y...}
# map21 {M y}		: from y get {x...}
#

#populate the map M with a single x-y pairing labeled r
proc map {M x y r} {
    upvar #0 MAP_[set M]_12_3 MAP_12_3
    upvar #0 MAP_[set M]_m2_1 MAP_m2_1
    if {[info exists MAP_m2_1]} {
	    puts stderr "flushing crunched MAP data for new values"
            upvar #0 MAP_[set M]_m1_2 MAP_m1_2
            upvar #0 MAP_[set M]_m2_3 MAP_m2_3
            upvar #0 MAP_[set M]_m1_3 MAP_m1_3
            upvar #0 MAP_[set M]_3_m1 MAP_3_m1
            upvar #0 MAP_[set M]_3_m2 MAP_3_m2
            upvar #0 MAP_[set M]_2_m1 MAP_2_m1
            upvar #0 MAP_[set M]_1_m2 MAP_1_m2
	    array unset MAP_m2_1
	    array unset MAP_m1_2
            array unset MAP_m2_3
            array unset MAP_m1_3
            array unset MAP_3_m1
            array unset MAP_3_m2
            array unset MAP_2_m1
            array unset MAP_1_m2
    }
    lappend MAP_12_3([list $x $y]) $r
}

#lazily crunch the map
proc mapc {M map} {
    switch $map {
	"12-3" {
            upvar #0 MAP_[set M]_12_3 MAP_12_3
	    if {! [info exists MAP_12_3]} {
	        puts stderr "MAP_[set M]_12_3 doesn't exist.  Use the "map" proc to create and populate."	
	    }
	}
        m2_1 - m1_2 - m2_3 - m1_3 {
            upvar #0 MAP_[set M]_12_3 MAP_12_3
            upvar #0 MAP_[set M]_m2_1 MAP_m2_1
            upvar #0 MAP_[set M]_m1_2 MAP_m1_2
            upvar #0 MAP_[set M]_m2_3 MAP_m2_3
            upvar #0 MAP_[set M]_m1_3 MAP_m1_3
            #obtain sorted r_set's and use them as keys to the map
            foreach {xy} [array names MAP_12_3] {
                foreach {x y} $xy {break}
                set r_set [lsort -unique $MAP_12_3($xy)]
                set m1 [list $x $r_set]
                set m2 [list $y $r_set]
                lappend MAP_m2_1($m2) $x
                lappend MAP_m1_2($m1) $y
                lappend MAP_m2_3($m2) $r_set
                lappend MAP_m1_3($m1) $r_set
            }
        }
        1_m2 {
            upvar #0 MAP_[set M]_m2_1 MAP_m2_1
            upvar #0 MAP_[set M]_1_m2 MAP_1_m2
    	    if {! [info exists MAP_m2_1]} {mapc $M m2_1}
            #set up for finding m2 from x, and for listing x
            foreach {m2} [lsort [array names MAP_m2_1]] {
                foreach {x} $MAP_m2_1($m2) {
                    lappend MAP_1_m2($x) $m2
                }
            }    
        }
        2_m1 {
            upvar #0 MAP_[set M]_m1_2 MAP_m1_2
            upvar #0 MAP_[set M]_2_m1 MAP_2_m1
    	    if {! [info exists MAP_m2_1]} {mapc $M m2_1}
    	    if {! [info exists MAP_m1_2]} {mapc $M m1_2}
            #set up for finding m1 from y, and for listing y
            foreach {m1} [lsort [array names MAP_m1_2]] {
                foreach {y} $MAP_m1_2($m1) {
                    lappend MAP_2_m1($y) $m1
                }
            }
        }
        3_m2 {
            upvar #0 MAP_[set M]_m2_3 MAP_m2_3
            upvar #0 MAP_[set M]_3_m2 MAP_3_m2
    	    if {! [info exists MAP_m2_3]} {mapc $M m2_3}
            #set up for finding m2 from r_sets, and for listing r_sets
            foreach {m2} [lsort [array names MAP_m2_3]] {
		foreach {r_set} [lsort -unique $MAP_m2_3($m2)] {
        	    lappend MAP_3_m2($r_set) $m2
                }
            }
        }
        3_m1 {
            upvar #0 MAP_[set M]_m1_3 MAP_m1_3
            upvar #0 MAP_[set M]_3_m1 MAP_3_m1
    	    if {! [info exists MAP_m1_3]} {mapc $M m1_3}
            #set up for finding m1 from r_sets, and for listing r_sets
            foreach {m1} [lsort [array names MAP_m1_3]] {
		foreach {r_set} [lsort -unique $MAP_m1_3($m1)] {
        	    lappend MAP_3_m1($r_set) $m1
                }
            }
        }
    }
}
    
proc map1 {M} { ;#get {x...}
    upvar #0 MAP_[set M]_1_m2 MAP_1_m2
    if {! [info exists MAP_1_m2]} {mapc $M 1_m2}
    lsort [array names MAP_1_m2]
}
proc map2 {M} { ;#get {y...}
    upvar #0 MAP_[set M]_2_m1 MAP_2_m1
    if {! [info exists MAP_2_m1]} {mapc $M 2_m1}
    lsort [array names MAP_2_m1]
}
proc map3 {M} { ;#get {r...}
    # the set of r is the same in both MAP_3_m1 and MAP_3_m2, so just use one
    upvar #0 MAP_[set M]_3_m1 MAP_3_m1
    if {! [info exists MAP_3_m1]} {mapc $M 3_m1}
    lsort [array names MAP_3_m1]
}
proc mapm1 {M} { ;#get {{x {r...}}...}
    upvar #0 MAP_[set M]_m1_2 MAP_m1_2
    if {! [info exists MAP_m1_2]} {mapc $M m1_2}
    lsort [array names MAP_m1_2]
}
proc mapm2 {M} { ;#get {{y {r...}}...}
    upvar #0 MAP_[set M]_m2_1 MAP_m2_1
    if {! [info exists MAP_m2_1]} {mapc $M m2_1}
    lsort [array names MAP_m2_1]
}
proc map1m2 {M x} { ;#from x get {y {r...}}
    upvar #0 MAP_[set M]_1_m2 MAP_1_m2
    if {! [info exists MAP_1_m2]} {mapc $M 1_m2}
    set MAP_1_m2($x)
}
proc map2m1 {M y} { ;#from y get {x {r...}}
    upvar #0 MAP_[set M]_2_m1 MAP_2_m1
    if {! [info exists MAP_2_m1]} {mapc $M 2_m1}
    set MAP_2_m1($y)
}
proc map3m1 {M r_set} { ;#from r_set get {{x {r...}}...}
    upvar #0 MAP_[set M]_3_m1 MAP_3_m1
    if {! [info exists MAP_3_m1]} {mapc $M 3_m1}
    set MAP_3_m1($r_set)
}
proc map3m2 {M r_set} { ;#from r_set get {{y {r...}}...}
    upvar #0 MAP_[set M]_3_m2 MAP_3_m2
    if {! [info exists MAP_3_m2]} {mapc $M 3_m2}
    set MAP_3_m2($r_set)
}
proc mapm21 {M m2} { ;#from m2 get {x...}
    upvar #0 MAP_[set M]_m2_1 MAP_m2_1
    if {! [info exists MAP_m2_1]} {mapc $M m2_1}
    lsort -unique $MAP_m2_1($m2)
}
proc mapm12 {M m1} { ;#from m1 get {y...}
    upvar #0 MAP_[set M]_m1_2 MAP_m1_2
    if {! [info exists MAP_m1_2]} {mapc $M m1_2}
    lsort -unique $MAP_m1_2($m1)
}
proc map12 {M x} { ;#from x get {y...}
    upvar #0 MAP_[set M]_1_m2 MAP_1_m2
    if {! [info exists MAP_1_m2]} {mapc $M 1_m2}
    set res [list]
    foreach {m} $MAP_1_m2($x) {
	foreach {y r_set} $m {break}
        lappend res $y
    }
    lsort -unique $res
}
proc map21 {M y} { ;#from y get {x...}
    upvar #0 MAP_[set M]_2_m1 MAP_2_m1
    if {! [info exists MAP_2_m1]} {mapc $M 2_m1}
    set res [list]
    foreach {m} $MAP_2_m1($y) {
	foreach {x r_set} $m {break}
        lappend res $x
    }
    lsort -unique $res
}

proc map_debug {M} {
    foreach {map} [list 12_3 m2_1 m1_2 m2_3 m1_3 1_m2 2_m1 3_m1 3_m2] {
	upvar #0 MAP_[set M]_$map MAP_$map
        if {! [info exists MAP_$map]} {mapc $M $map}
	puts "MAP_[set M]_$map :"
        print_first MAP_[set M]_$map
    }
}

if {0} {  ;# for testing
	map CV black 0 svg
	map CV black 0 x11
	map CV black 0 tk
	map CV grey0 0 svg
	map CV grey0 0 x11
	map CV grey0 0 tk
	map CV noir 0 french
	map CV green 2 svg
	map CV lime 1 svg
	map CV green 1 x11
	map CV green 1 tk
	map CV lime  1 tk
	map CV lime  1 x11
	map CV vert 1 french

	puts [info vars]
	puts ""
	map_debug CV
	puts ""
puts "m1_3 : [array get MAP_CV_m1_3]"
puts "3_m1 : [array get MAP_CV_3_m1]"

	puts "map1 CV                : [map1 CV]"
	puts "              expected : black green grey0 lime noir vert"
	puts "map2 CV                : [map2 CV]"
       	puts "              expected : 0 1 2"
	puts "map3 CV                : [map3 CV]"
       	puts "              expected : french svg {svg tk x11} {tk x11}"
	puts "map1m2 CV green        : [map1m2 CV green]"
	puts "              expected : {1 {tk x11}} {2 svg}"
	puts "map2m1 CV 2            : [map2m1 CV 2]"
	puts "              expected : {green svg}"
	puts "map3m1 CV {svg tk x11} : [map3m1 CV {svg tk x11}]"
	puts "              expected : {black {svg tk x11}} {grey0 {svg tk x11}} {lime {svg tk x11}}"
	puts "map3m2 CV {svg tk x11} : [map3m2 CV {svg tk x11}]"
       	puts "              expected : {0 {svg tk x11}} {1 {svg tk x11}}"
	puts "map3m2 CV {svg}        : [map3m2 CV {svg}]"
       	puts "              expected : {2 {svg}}"
	puts "map12 CV green         : [map12 CV green]"
	puts "              expected : 1 2"
	puts "map21 CV 1             : [map21 CV 1]"
	puts "              expected : green lime vert"
	puts "mapm12 CV {green svg}  : [mapm12 CV {green svg}]"
	puts "              expected : 2"
	puts "mapm21 CV {0 {svg tk x11}} : [mapm21 CV {0 {svg tk x11}}]"
	puts "              expected : black grey0"
}
