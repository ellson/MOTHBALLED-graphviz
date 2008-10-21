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
            set s "$indent/* [uplevel 2 $rstart] */"
            set w [expr 16 - [string length $s]]
            if {$w < 0} {set w 0}
            set s $s[format "%.[set w]s" "               "]
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
	if {$pos > 40} {
	    puts -nonewline $ch "\n\t\t\t\t\t"
	}
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

proc tab_elem {data} {
    global comments
    upvar TAB_pos         pos
    upvar TAB_chan        ch
    if {$pos > 72} { 
	if {$comments} {
	    puts -nonewline $ch "\n\t\t"
	    set pos 16
        } {
	    puts $ch ""
	    set pos 0
        }
    }
    if {$pos == 0} { tab_start_row }
    puts -nonewline $ch $data
    incr pos [string length $data]
}

proc tab_row { } {
    upvar TAB_pos         pos
    if {$pos} { tab_end_row }
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
# setfrom pattern struct
#
# recursive proc for unpacking pseudo-structs with llength checks

proc setfrom {pattern struct {d_ {1}}} {
    set l_ [llength $pattern]
    if {$l_ != [llength $struct]} {
        puts stderr "setfrom: llengths don't match: \"$pattern\" \"$struct\""
        exit
    }
    for {set i_ 0} {$i_ < $l_} {incr i_} {
	set p_ [lindex $pattern $i_]
	set s_ [lindex $struct $i_]
        if {[llength $p_] > 1} {
            setfrom $p_ $s_ [expr $d_ + 1]
	} {
	    upvar $d_ $p_ $p_
	    set $p_ $s_
	}
    }
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
# mapm1 {M}		: get {{x {r...}}...}
# mapm2 {M} 		: get {{y {r...}}...}
# map1m2 {M x}		: from x get {y {r...}}
# map2m1 {M y}		: from y get {x {r...}}
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
            upvar #0 MAP_[set M]_2_m1 MAP_2_m1
            upvar #0 MAP_[set M]_1_m2 MAP_1_m2
	    array unset MAP_m2_1
	    array unset MAP_m1_2
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
        m2_1 - m1_2 {
            upvar #0 MAP_[set M]_12_3 MAP_12_3
            upvar #0 MAP_[set M]_m2_1 MAP_m2_1
            upvar #0 MAP_[set M]_m1_2 MAP_m1_2
            foreach {xy} [array names MAP_12_3] {
                setfrom {x y} $xy
                set r_set [lsort -unique $MAP_12_3($xy)]
                set m1 [list $x $r_set]
                set m2 [list $y $r_set]
                lappend MAP_m2_1($m2) $x
                lappend MAP_m1_2($m1) $y
            }
        }
        1_m2 {
            upvar #0 MAP_[set M]_m2_1 MAP_m2_1
            upvar #0 MAP_[set M]_1_m2 MAP_1_m2
    	    if {! [info exists MAP_m2_1]} {mapc $M m2_1}
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
            foreach {m1} [lsort [array names MAP_m1_2]] {
                foreach {y} $MAP_m1_2($m1) {
                    lappend MAP_2_m1($y) $m1
                }
            }
        }
    }
}
    
proc map1 {M} { ;#get {x...}
    upvar #0 MAP_[set M]_1_m2 MAP_1_m2
    if {! [info exists MAP_1_m2]} {mapc $M 1_m2}
    array names MAP_1_m2
}
proc map2 {M} { ;#get {y...}
    upvar #0 MAP_[set M]_2_m1 MAP_2_m1
    if {! [info exists MAP_2_m1]} {mapc $M 2_m1}
    array names MAP_2_m1
}
proc mapm1 {M} { ;#get {{x {r...}}...}
    upvar #0 MAP_[set M]_m1_2 MAP_m1_2
    if {! [info exists MAP_m1_2]} {mapc $M m1_2}
    array names MAP_m1_2
}
proc mapm2 {M} { ;#get {{y {r...}}...}
    upvar #0 MAP_[set M]_m2_1 MAP_m2_1
    if {! [info exists MAP_m2_1]} {mapc $M m2_1}
    array names MAP_m2_1
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
	setfrom {y r_set} $m
        lappend res $y
    }
    lsort -unique $res
}
proc map21 {M y} { ;#from y get {x...}
    upvar #0 MAP_[set M]_2_m1 MAP_2_m1
    if {! [info exists MAP_2_m1]} {mapc $M 2_m1}
    set res [list]
    foreach {m} $MAP_2_m1($y) {
	setfrom {x r_set} $m
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

	puts "map1 CV                : [lsort [map1 CV]]"
	puts "              expected : black green grey0 lime noir vert"
	puts "map2 CV                : [lsort [map2 CV]]"
       	puts "              expected : 0 1 2"
	puts "map1m2 CV green        : [map1m2 CV green]"
	puts "              expected : {1 {tk x11}} {2 svg}"
	puts "map2m1 CV 2            : [map2m1 CV 2]"
	puts "              expected : {green svg}"
	puts "map12 CV green         : [map12 CV green]"
	puts "              expected : 1 2"
	puts "map21 CV 1             : [map21 CV 1]"
	puts "              expected : green lime vert"
	puts "mapm12 CV {green svg}  : [mapm12 CV {green svg}]"
	puts "              expected : 2"
	puts "mapm21 CV {0 {svg tk x11}} : [mapm21 CV {0 {svg tk x11}}]"
	puts "              expected : black grey0"
}
