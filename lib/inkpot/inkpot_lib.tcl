#!/usr/bin/tclsh

set comments 1

set preamble "/*\n * DO NOT EDIT !!\n *\n * The files:\n *\tinkpot_lib_define.h\n *\tinkpot_lib_value.h\n *\tinkpot_lib_scheme.h\n *\tinkpot_lib_string.h\n * are generated as a matched set.\n */\n"

if {! $argc} {exit}

source inkpot_lib_procs.tcl

#------------------------------------------------- set_up
# merge input data
foreach {lib} $argv {
    set f [open $lib r]
    foreach {color value} [read $f [file size $lib]] {
	set COLORS($color) $value
    }
    close $f

    foreach {scheme coding} $COLORS() {break}
    array unset COLORS {}
    set ALL_STRINGS($scheme) {}
    set ALL_SCHEMES($scheme) {}

    foreach {color} [array names COLORS] {
        set value $COLORS($color)

	# if a named color ends in a number, convert it to an indexed color
        if {[llength $color] == 1
        && [regexp -- {([a-z]+)([0-9]+)} $color . icolor index] == 1} {
            unset COLORS($color)
            set color [list $icolor $index]
            set COLORS($color) $value
        }

        switch [llength $color] {
            1 {
		set icolor $color
		set range 1
		set index 0
            }
            2 {
                foreach {icolor index} $color {break}
		set range 0
            }
            3 {
                foreach {icolor range index} $color {break}
		if {$range == 1} {
		    puts stderr "Weird.  Unexpected range==1. I guess that means I can't use 1 as a flag! $color $value $scheme"
		    exit
		}
            }
            default {
                puts stderr "Weird! Too many fields in: \"$color\""
		exit 
            }
        }
	map C_RIV $icolor [list $range $index $value] $scheme
        set ALL_STRINGS($icolor) {}
    }
    array unset COLORS
}

#------------------------------------------------- crunch some data ----------

# find all unique color_schemeset_range and list index_value
# find all unique value and list color_schemeset_range_index
foreach {m2} [mapm2 C_RIV] {
    foreach {RIV schemes} $m2 {break}
    foreach {range index value} $RIV {break}
    foreach {m1} [map2m1 C_RIV $RIV] {
	lappend ALL_COLORSCHEMESRANGES([list $m1 $range]) [list $index $value]
    	lappend ALL_VALUES($value) [list $m1 $range $index]
    }
}
#share rangesets between aliases such as grey/0-100 and gray/0-100
foreach {m1_range} [array names ALL_COLORSCHEMESRANGES] {
    set rangeset [concat [lsort -dictionary -unique $ALL_COLORSCHEMESRANGES($m1_range)]]
    lappend ALL_RANGESETS($rangeset) $m1_range
}

#------------------------------------------------- write inkpot_lib_value.h
set f [open inkpot_lib_value.h w]
puts $f $preamble


proc format_color {color range index} {
    switch $range {
	0 {
	    set color $color$index
	}
	1 {
	    if {$index} {
		puts stderr "Weird! Color with range == 1 but index != 0. $color/$range/$index"
		exit
	    }
	    # otherwise just $color
	}
	default {
	    set color $color$range/$index
	}
    }
    return $color
}

# generate TAB_VALUES_24
tab_initialize $f {set SZT_VALUES} {set comment} \
	"unsigned char TAB_VALUES_24\[SZT_VALUES_24\] = {"
set SZT_VALUES 0
foreach {value} [lsort -dictionary [array names ALL_VALUES]] {
    foreach {r g b} $value {break}
    tab_elem [format "0x%02x,0x%02x,0x%02x," $r $g $b]
    set comment [list]
    foreach {m1_range_index} [lsort -unique $ALL_VALUES($value)] {
        foreach {m1 range index} $m1_range_index {break}
	foreach {color schemes} $m1 {break}
	lappend comment [list $schemes [format_color $color $range $index]]
    }
    tab_row
    set ALL_VALUES_coded($value) $SZT_VALUES
    incr SZT_VALUES
}
tab_finalize "};\n"
    
close $f
#------------------------------------------------- write inkpot_lib_string.h
set f [open inkpot_lib_string.h w]
puts $f $preamble

# generate TAB_STRINGS
set SZT_STRINGS 0
set SZL_STRINGS 0
set SZW_STRINGS 0
tab_initialize $f {set SZT_STRINGS} {set len} \
	"const char TAB_STRINGS\[SZT_STRINGS\] = {"
foreach {string} [lsort -ascii [array names ALL_STRINGS]] {
    set len [string length $string]
    # include the null
    incr len
    tab_elem "\"$string\\0\""
    tab_row
    set ALL_STRINGS_coded($string) $SZT_STRINGS
    incr SZW_STRINGS
    if {$len > $SZL_STRINGS} {set SZL_STRINGS $len}
    incr SZT_STRINGS $len
}
tab_finalize "};\n"
# don't count the null in the length of the longest string
incr SZL_STRINGS -1

close $f
#------------------------------------------------- write inkpot_lib_scheme.h
set f [open inkpot_lib_scheme.h w]
puts $f $preamble


# generate TAB_SCHEMES
set SZT_SCHEMES 0
tab_initialize $f {set SZT_SCHEMES} {set scheme} \
	"inkpot_scheme_t TAB_SCHEMES\[SZT_SCHEMES\] = {"
foreach {scheme} [lsort -ascii [array names ALL_SCHEMES]] {
    tab_elem $ALL_STRINGS_coded($scheme),
    tab_row
    set ALL_SCHEMES_coded($scheme) [list $SZT_SCHEMES [expr 1 << $SZT_SCHEMES]]
    incr SZT_SCHEMES
}
tab_finalize "};\n"


# generate TAB_INDEXES
set SZT_INDEXES 0
set index -1
tab_initialize $f {set SZT_INDEXES} {set comment} \
	"IDX_VALUES_t TAB_INDEXES\[SZT_INDEXES\] = {"
foreach {rangeset} [lsort -dictionary [array names ALL_RANGESETS]] {
    set size [llength $rangeset]
    if {$size == 1} {
        #simple non-indexed color
	foreach {index_value} $rangeset {break}
	foreach {index value} $index_value {break}
	if {$index} {
	    puts stderr "Weird! Non-zero index $index for non-indexed color"
	    exit
	}
	foreach {m1_range} $ALL_RANGESETS($rangeset) {
	    foreach {m1 range} $m1_range {break}
	    foreach {color schemes} $m1 {break}
	    if {$range != 1} {
		puts stderr "Weird! Range $range != 1 for non-indexed color"
		exit
	    }
	    lappend ALL_ALTS($color) [list $value $schemes]
	}
	continue
    }
    tab_row
    set found 0
    set first_szt_index $SZT_INDEXES
    foreach {index_value} $rangeset {
	foreach {index value} $index_value {break}
	if {! $found} {
	    set min_index $index
	    set max_index $index
	    incr found
        } {
	    if {$index < $min_index} {set min_index $index}
	    if {$index > $max_index} {set max_index $index}
	}
        tab_elem $ALL_VALUES_coded($value),
        incr SZT_INDEXES
    }
    set last_index $index
    set comment [list]
    foreach {m1_range} $ALL_RANGESETS($rangeset) {
	foreach {m1 range} $m1_range {break}
	if {$range} {
	    if {$range != $size} {
	        puts stderr "Weird! Range doesn't match size. $m1 $range != $size"
		exit
	    }
        } {
	    set range $size
        }
        foreach {color schemes} $m1 {break}
        lappend comment [list $schemes $color/$range/$min_index-$max_index]
        set ALL_RANGES_coded([list $m1 $range]) [list $first_szt_index $size]
    }
}
tab_finalize "};\n"

# generate TAB_RANGES
set SZT_RANGES 0
set last_color ""
tab_initialize $f {set SZT_RANGES} {list $schemes $color $cnt} \
	"inkpot_range_t TAB_RANGES\[SZT_RANGES\] = {"
foreach {m1_range} [lsort -ascii [array names ALL_RANGES_coded]] {
    foreach {m1 range} $m1_range {break}
    foreach {color schemes} $m1 {break}
    foreach {first_idx size} $ALL_RANGES_coded($m1_range) {break}
    if {! [string equal $last_color $color]} {
	tab_row
	set cnt 0
	set last_color $color
	set ALL_ALTS_coded($color) $SZT_RANGES
    }
    set scheme_bits 0
    foreach {scheme} $schemes {
        foreach {scheme_idx scheme_bit} $ALL_SCHEMES_coded($scheme) {break}
        set scheme_bits [expr $scheme_bits | $scheme_bit]
    }
    tab_elem "{$size,$first_idx,[format {0x%x} $scheme_bits]},"
    incr SZT_RANGES
    incr cnt
}
tab_finalize "};\n"

#accumulate altsets for all non-indexed colors
foreach {color} [lsort -ascii [array names ALL_ALTS]] {
    lappend ALL_ALTSETS($ALL_ALTS($color)) $color
}

# generate TAB_ALTS
set SZT_ALTS 0
tab_initialize $f {set first_idx} {set aliases} \
	"inkpot_name_t TAB_ALTS\[SZT_ALTS\] = {"
foreach {altset} [lsort -dictionary [array names ALL_ALTSETS]] {
    set aliases [lsort -ascii -unique $ALL_ALTSETS($altset)]
    set cnt [llength $altset]
    switch $cnt {
        0 {
            puts stderr "shouldn't happen - has to be at least one alt in an altset"
        }
        1 {
	    foreach {value_schemes} $altset {break}
	    foreach {value schemes} $value_schemes {break}
	    set scheme_bits 0
    	    foreach {scheme} $schemes {
                foreach {scheme_idx scheme_bit} $ALL_SCHEMES_coded($scheme) {break}
                set scheme_bits [expr $scheme_bits | $scheme_bit]
            }
	    foreach {color} $aliases {
                set ALL_ALTS_coded($color) "$ALL_VALUES_coded($value),[format {0x%x} $scheme_bits]"
	    }
            # don't need entry in TAB_ALTS for this case
        }
        default {
            set first_idx $SZT_ALTS
	    foreach {value_schemes} $altset {
	        foreach {value schemes} $value_schemes {break}
	        set scheme_bits 0
    	        foreach {scheme} $schemes {
                    foreach {scheme_idx scheme_bit} $ALL_SCHEMES_coded($scheme) {break}
                    set scheme_bits [expr $scheme_bits | $scheme_bit]
                }
                tab_elem "{[incr cnt -1],$ALL_VALUES_coded($value),[format {0x%x} $scheme_bits]},"
                incr SZT_ALTS
    	        foreach {color} $aliases {
                    set ALL_ALTS_coded($color) "$first_idx,0"
                }
    	    }
        }
    }
    tab_row
}
tab_finalize "};\n"

# generate TAB_NAMES
set SZT_NAMES 0
tab_initialize $f {set SZT_NAMES} {set color} \
	"inkpot_name_t TAB_NAMES\[SZT_NAMES\] = {"
foreach {color} [map1 C_RIV] {
    tab_elem "{$ALL_STRINGS_coded($color),$ALL_ALTS_coded($color)},"
    tab_row
    set ALL_NAMES_coded($color) $SZT_NAMES
    incr SZT_NAMES
}
tab_finalize "};\n"
    

# generate TAB_TO_NAMES
set SZT_TO_NAMES 0
tab_initialize $f {set SZT_TO_NAMES} {set alias_set} \
	"IDX_NAMES_t TAB_TO_NAMES\[SZT_TO_NAMES\] = {"
foreach {m2} [mapm2 C_RIV] {
    set alias_set [mapm21 C_RIV $m2]
    foreach {value schemeset} $m2 {break}
    set ALL_TO_NAMES_coded($value) $SZT_TO_NAMES
    set first_idx $SZT_TO_NAMES
    foreach {color} $alias_set {
       	tab_elem $ALL_NAMES_coded($color),
       	lappend comment $color
       	incr SZT_TO_NAMES
    }
    tab_row
}
tab_finalize "};\n"


# generate TAB_VALUE_TO
set SZT_VALUE_TO 0
tab_initialize $f {set SZT_VALUE_TO} {set ALL_TO_NAMES_coded($value)} \
	"IDX_TO_NAMES_t TAB_VALUE_TO\[SZT_VALUE_TO\] = {"
# NB - this sort order must match TAB_VALUES
foreach {value} [lsort -dictionary [map2 C_RIV]] {
    tab_elem $ALL_TO_NAMES_coded($value),
    tab_row
    incr SZT_VALUE_TO
}
tab_finalize "};\n"
    

close $f
#------------------------------------------------- write inkpot_lib_define.h
set f [open inkpot_lib_define.h w]
puts $f $preamble

set SZT_VALUES_24 [expr 3*$SZT_VALUES]

puts $f "\#define SZL_STRINGS $SZL_STRINGS"
puts $f "\#define SZW_STRINGS $SZW_STRINGS"
puts $f ""
foreach {i} {
    STRINGS SCHEMES NAMES ALTS VALUES VALUE_TO TO_NAMES
    INDEXES RANGES VALUES_24
} {
    if {[set SZT_$i] < 256} {
        set int "unsigned char"
    } elseif {[set SZT_$i] < 65536} {
        set int "unsigned short"
    } elseif {[set SZT_$i] < 65536*65536} {
        set int "unsigned int"
    } else {
        set int "unsigned long"
    }
    puts $f "\#define SZT_$i [set SZT_$i]"
    puts $f "typedef $int IDX_[set i]_t\;"
    puts $f ""
}
foreach {i} {SCHEMES} {
    if {[set SZT_$i] < 8} {
        set int "unsigned char"
    } elseif {[set SZT_$i] < 16} {
        set int "unsigned short"
    } elseif {[set SZT_$i] < 32} {
        set int "unsigned int"
    } elseif {[set SZT_$i] < 64} {
        set int "unsigned long"
    } else {
        puts stderr "more that 64 bits in MSK_[set i]_t"
    }
    puts $f "typedef $int MSK_[set i]_t\;"
    puts $f ""
}
close $f
