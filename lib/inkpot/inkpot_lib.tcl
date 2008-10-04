
set comments 1
set target_line_length 60
set indent "  "

set preamble "/*\n * DO NOT EDIT !!\n *\n * The files:\n *\tinkpot_define.h\n *\tinkpot_value_table.h\n *\tinkpot_scheme_table.h\n * are generated as a matched set.\n */\n"

if {! $argc} {exit}

source inkpot_lib_procs.tcl

#------------------------------------------------- set_up
# merge input data
foreach {lib} $argv {
    set f [open $lib r]
    array set COLORS [read $f [file size $lib]]
    close $f

    foreach {scheme coding} $COLORS() {break}
    array unset COLORS {}
    lappend ALL_STRINGS($scheme) scheme

    foreach {color} [array names COLORS] {
        set value $COLORS($color)

        if {[llength $color] == 1
        && [regexp -- {([a-z]+)([0-9]+)} $color . subscheme index] == 1} {
            unset COLORS($color)
            set color [list $subscheme $index]
            set COLORS($color) $value
        }

        switch [llength $color] {
            1 {
                lappend ALL_STRINGS($color) color
		map C V $color $value $scheme
            }
            2 {
                foreach {subscheme index} $color {break}
                lappend ALL_STRINGS($subscheme) subscheme
		map I V $index $value [list $scheme $subscheme 0]
#old
                lappend ALL_INDEX_SCHEMES([list $scheme $subscheme 0]) $index $value
                lappend ALL_INDEXES($value) $coding $scheme $subscheme 0 $index
            }
            3 {
                foreach {subscheme range index} $color {break}
                lappend ALL_STRINGS($subscheme) subscheme
		map I V $index $value [list $scheme $subscheme $range]
#old
                lappend ALL_INDEX_SCHEMES([list $scheme $subscheme $range]) $index $value
                lappend ALL_INDEXES($value) $coding $scheme $subscheme $range $index
            }
            default {
                puts stderr "wrong number of keys in: \"$color\""
            }
        }
    }
}

# crunch the data
mapc C V
mapc I V

foreach {v} [map2 I V] {
    foreach {m} [map2m I V $v] {
	foreach {index scheme_subscheme_range} $m {
	    foreach {scheme subscheme range} $scheme_subscheme_range {break}
	    map RI V [list $range $index] $v [list $scheme $subscheme]
	}
    }
}
mapc RI V

foreach {v} [map2 RI V] {
    foreach {m} [map2m RI V $v] {
	foreach {index scheme_subscheme} $m {
            foreach {scheme subscheme} $scheme_subscheme {break}
	    map SRI V [list $subscheme $range $index] $v $scheme
	}
    }
}
mapc SRI V

foreach {index_scheme} [lsort -ascii [array names ALL_INDEX_SCHEMES]] {
    foreach {index value} $ALL_INDEX_SCHEMES($index_scheme) {
        set indexes($index) $value
    }
    foreach {index} [lsort -dictionary [array names indexes]] {
        lappend valueset $indexes($index)
    }
    lappend ALL_IDXSETS($valueset) $index_scheme
    array unset indexes
    unset valueset
}

#------------------------------------------------- write inkpot_value_table.h
set f [open inkpot_value_table.h w]
puts $f $preamble

# generate TAB_VALUES_24
set SZT_VALUES 0
tab_begin $f "unsigned char TAB_VALUES_24\[SZT_VALUES_24\] = {"
foreach {value} [map2 C V] {
    tab_begin_block $f $SZT_VALUES

    foreach {r g b} $value {break}
    tab_elem $f [format "0x%02x,0x%02x,0x%02x," $r $g $b]

    set ALL_VALUES_coded($value) $SZT_VALUES
    incr SZT_VALUES
    
    tab_end_block $f [map2m C V $value]
}
tab_end $f "};\n"

# generate NONAME_VALUES_24
set SZT_NONAME_VALUES 0
tab_begin $f "unsigned char TAB_NONAME_VALUES_24\[SZT_NONAME_VALUES_24\] = {"
foreach {value} [map2 I V] {
    if {! [info exists ALL_VALUES($value)]} {
        tab_begin_block $f $SZT_NONAME_VALUES
    
        foreach {r g b} $value {break}
        tab_elem $f [format "0x%02x,0x%02x,0x%02x," $r $g $b]

        set ALL_VALUES_coded($value) [expr $SZT_NONAME_VALUES + $SZT_VALUES]
        incr SZT_NONAME_VALUES

if {1} {
        foreach {coding scheme subscheme range index} $ALL_INDEXES($value) {
            set sri [list $subscheme $range $index]
            lappend SRI($sri) $scheme
        }
        set comment [list]
        foreach {sri} [lsort -ascii [array names SRI]] {
            foreach {subscheme range index} $sri {break}
            foreach {scheme} $SRI($sri) {
                set schemes($scheme) {}
            }
            set schemes_s "\([join [lsort -ascii [array names schemes]] ,]\)"
            array unset schemes
            if {$range} {
                lappend comment "$schemes_s$subscheme$range<$index>"
            } {
                lappend comment "$schemes_s$subscheme<$index>"
            }
        }
        unset SRI
    
        tab_end_block $f $comment
}


#    	tab_end_block $f [map2m RI V $value]
    }
}
tab_end $f "};\n"
    
close $f
#------------------------------------------------- write inkpot_scheme_table.h
set f [open inkpot_scheme_table.h w]
puts $f $preamble

# generate TAB_STRINGS
set SZT_STRINGS 0
set SZL_STRINGS 0
set SZW_STRINGS 0

tab_begin $f "const char TAB_STRINGS\[SZT_STRINGS\] = {"
foreach {string} [lsort -ascii [array names ALL_STRINGS]] {
    tab_begin_block $f $SZT_STRINGS
    
    tab_elem $f "\"$string\\0\""

    set len [string length $string]
    # include the null
    incr len

    tab_end_block $f $len
    
    foreach {usage} $ALL_STRINGS($string) {
        switch $usage {
            scheme {
                set ALL_SCHEME_STRINGS_coded($string) $SZT_STRINGS
            }
            subscheme {
                set ALL_SUBSCHEME_STRINGS_coded($string) $SZT_STRINGS
            }
            color {
                set ALL_COLOR_STRINGS_coded($string) $SZT_STRINGS
            }
            default {
                puts stderr "Unknown usage $usage for string \"$string\""
            }
        }
    }
    incr SZW_STRINGS
    if {$len > $SZL_STRINGS} {set SZL_STRINGS $len}
    incr SZT_STRINGS $len
}
tab_end $f "};\n"
# don't count the null in the length of the longest string
incr SZL_STRINGS -1


# generate TAB_INDEXES
set SZT_INDEXES 0
tab_begin $f "IDX_VALUES TAB_INDEXES\[SZT_INDEXES\] = {"
foreach {valueset} [lsort [array names ALL_IDXSETS]] {
    set first_idx $SZT_INDEXES
    tab_begin_block $f $SZT_INDEXES
    foreach {value} $valueset {
        tab_elem $f $ALL_VALUES_coded($value),
        incr SZT_INDEXES
    }
    set comment [list]
    foreach {index_scheme} $ALL_IDXSETS($valueset) {
        set ALL_INDEX_RANGES_coded($index_scheme) [list $first_idx [expr $SZT_INDEXES - $first_idx]]
        foreach {scheme subscheme range} $index_scheme {break}
        if {$range} {
            lappend comment $scheme/$subscheme$range
        } {
            lappend comment $scheme/$subscheme
        }
    }
    tab_end_block $f $comment
}
tab_end $f "};\n"

foreach {index_scheme} [lsort [array names ALL_INDEX_RANGES_coded]] {
    foreach {scheme subscheme range} $index_scheme {break}
    foreach {first_idx size} $ALL_INDEX_RANGES_coded($index_scheme) {break}
    lappend ALL_INDEX_SUBSCHEME_map([list $subscheme $first_idx $size]) $scheme
}
foreach {subscheme_first_idx_size} [array names ALL_INDEX_SUBSCHEME_map] {
    foreach {subscheme first_idx size} $subscheme_first_idx_size {break}
    foreach {scheme} $ALL_INDEX_SUBSCHEME_map($subscheme_first_idx_size) {break}
    lappend ALL_INDEX_SUBSCHEMES_coded([list $scheme $subscheme]) $first_idx $size
}


# generate TAB_SUBSCHEMES_INDEX
set SZT_SUBSCHEMES_INDEX 0
tab_begin $f "inkpot_scheme_index_t TAB_SUBSCHEMES_INDEX\[SZT_SUBSCHEMES_INDEX\] = {"
foreach {scheme_subscheme} [lsort [array names ALL_INDEX_SUBSCHEMES_coded]] {
    foreach {scheme subscheme} $scheme_subscheme {break}

    tab_begin_block $f $SZT_SUBSCHEMES_INDEX

    foreach {first_idx size} $ALL_INDEX_SUBSCHEMES_coded($scheme_subscheme) {break}
    tab_elem $f "{$ALL_SUBSCHEME_STRINGS_coded($subscheme),$first_idx,$size},"

    incr SZT_SUBSCHEMES_INDEX

    tab_end_block $f "$scheme/$subscheme<1-$size>"
}
tab_end $f "};\n"

# generate TAB_SCHEMES_INDEX
set SZT_SCHEMES_INDEX 0
tab_begin $f "inkpot_scheme_index_t TAB_SCHEMES_INDEX\[SZT_SCHEMES_INDEX\] = {"
foreach {scheme_subscheme} [lsort [array names ALL_INDEX_SUBSCHEMES_coded]] {
    foreach {scheme subscheme} $scheme_subscheme {break}

    tab_begin_block $f $SZT_SCHEMES_INDEX
    set ALL_INDEX_SCHEMES_coded($scheme) $SZT_SUBSCHEMES_INDEX

    foreach {subschemes_idx} $ALL_INDEX_SUBSCHEMES_coded($scheme_subscheme) {
        tab_elem $f $subschemes_idx,
        incr SZT_SCHEMES_INDEX
    }

    if {$range} {
        tab_end_block $f $scheme/$subscheme$range
    } {
        tab_end_block $f $scheme/$subscheme
    }
}
tab_end $f "};\n"


# generate TAB_SCHEMES
set SZT_SCHEMES 0
tab_begin $f "inkpot_scheme_name_t TAB_SCHEMES\[SZT_SCHEMES\] = {"
foreach {scheme} [lsort -ascii [array names ALL_SCHEMES]] {
    tab_begin_block $f $SZT_SCHEMES
    
#    tab_elem $f "{$ALL_SCHEME_STRINGS_coded($scheme),$ALL_INDEX_SCHEMES_coded($scheme)},"
    tab_elem $f "{$ALL_SCHEME_STRINGS_coded($scheme)},"
    
    tab_end_block $f $scheme
    
    set ALL_SCHEMES($scheme) [list $SZT_SCHEMES [expr 1 << $SZT_SCHEMES]]
    incr SZT_SCHEMES
}
tab_end $f "};\n"

    
# generate TAB_ALTS
set SZT_ALTS 0
tab_begin $f "inkpot_name_t TAB_ALTS\[SZT_ALTS\] = {"
foreach {ms} [map1mas C V] {
    set isneeded 0
    set cnt [llength $ms]
    switch $cnt {
        0 {
            puts stderr "shouldn't happen - zero alts: $color"
        }
        1 {
            foreach {m} $ms {break}
	    foreach {value schemeset} $m {break}
	    set scheme_bits 0
            foreach {scheme} $schemeset {
                foreach {scheme_idx scheme_bit} $ALL_SCHEMES($scheme) {break}
                set scheme_bits [expr $scheme_bits | $scheme_bit]
            }
            set ALL_ALTSETS_coded($color) "$ALL_VALUES_coded($value),[format {0x%x} $scheme_bits]"
            # don't need entry in TAB_ALTS for this case
        }
        default {
            set first_idx $SZT_ALTS
	    foreach {m} $ms {
		foreach {value schemeset} $m {break}
	        set scheme_bits 0
                foreach {scheme} $schemeset {
                    foreach {scheme_idx scheme_bit} $ALL_SCHEMES($scheme) {break}
                    set scheme_bits [expr $scheme_bits | $scheme_bit]
                }
                tab_begin_block $f $first_idx
                incr isneeded
                tab_elem $f "{[incr cnt -1],$ALL_VALUES_coded($value),[format {0x%x} $scheme_bits]},"
                incr SZT_ALTS
	    }
	    foreach {color} $aliases {
                set ALL_ALTSETS_coded($color) "$first_idx,0"
            }
        }
    }
    if {$isneeded} {tab_end_block $f $aliases}
}
tab_end $f "};\n"

    
# generate TAB_NAMES
set SZT_NAMES 0
tab_begin $f "inkpot_name_t TAB_NAMES\[SZT_NAMES\] = {"
foreach {color} [lsort -ascii [array names ALL_ALTSETS]] {
    tab_begin_block $f $SZT_NAMES
    
    tab_elem $f "{$ALL_COLOR_STRINGS_coded($color),$ALL_ALTSETS_coded($color)},"
    
    tab_end_block $f $color
    
    set ALL_NAMES_coded($color) $SZT_NAMES
    incr SZT_NAMES
}
tab_end $f "};\n"
    

# generate TAB_TO_NAMES
set SZT_TO_NAMES 0
tab_begin $f "IDX_NAMES TAB_TO_NAMES\[SZT_TO_NAMES\] = {"
foreach {value} [map2 C V] {
    tab_begin_block $f $SZT_TO_NAMES
    set m [map2ma C V $value]
    switch [llength $m] {
        0 {
            puts stderr "shouldn't happen - zero maps: $value"
        }
	default {
            set first_idx $SZT_TO_NAMES
	    foreach {map} $m {
		foreach {color schemeset} $map {break}
        	tab_elem $f $ALL_NAMES_coded($color),
        	lappend comment $color
        	incr SZT_TO_NAMES
    	    }
        }
    }
    if {$isneeded} {tab_end_block $f \$aliases}
}
tab_end $f "};\n"

if {0}
# generate TAB_TO_NAMES
set SZT_TO_NAMES 0
tab_begin $f "IDX_NAMES TAB_TO_NAMES\[SZT_TO_NAMES\] = {"
foreach {value} [lsort -dictionary [array names ALL_VALUES]] {
    tab_begin_block $f $SZT_TO_NAMES

    set mapset $ALL_MAPSETS($value)
    set ALL_TO_NAMES_coded($value) $SZT_TO_NAMES

    set comment [list]
    foreach {color schemeset} $mapset {

        tab_elem $f $ALL_NAMES_coded($color),

        lappend comment $color
        incr SZT_TO_NAMES
    }

    tab_end_block $f $comment
}
tab_end $f "};\n"

if {$SZT_TO_NAMES != $SZT_NAMES} {
    puts stderr "That's weird! SZT_TO_NAMES $SZT_TO_NAMES != SZT_NAMES $SZT_NAMES"
}


# generate TAB_VALUE_TO
set SZT_VALUE_TO 0
tab_begin $f "IDX_TO_NAMES TAB_VALUE_TO\[SZT_VALUE_TO\] = {"
# NB - this sort order must match TAB_VALUES
foreach {value} [lsort -dictionary [array names ALL_VALUES]] {
    tab_begin_block $f $SZT_VALUE_TO

    set mapset $ALL_MAPSETS($value)
    tab_elem $f $ALL_TO_NAMES_coded($value),
    
    tab_end_block $f $ALL_TO_NAMES_coded($value)
    incr SZT_VALUE_TO
}
tab_end $f "};\n"
    
if {$SZT_VALUE_TO != $SZT_VALUES} {
    puts stderr "That's weird! SZT_VALUE_TO $SZT_VALUE_TO != SZT_VALUES $SZT_VALUES"
}

}

close $f


if {1} {
    puts stderr ""
    print_first ALL_STRINGS
    print_first ALL_NAMES_coded
    print_first ALL_ALTS
    print_first ALL_VALUES
    print_first ALL_ALTSETS
    print_first ALL_MAPSETS
    print_first ALL_ALTSET_COLORS
    print_first ALL_MAPSET_VALUES
    print_first ALL_VALUE_ALTSETS
    print_first ALL_COLOR_MAPSETS
    puts stderr ""
}

#------------------------------------------------- write inkpot_define.h
set f [open inkpot_define.h w]
puts $f $preamble

set SZT_VALUES_24 [expr 3*$SZT_VALUES]
set SZT_NONAME_VALUES_24 [expr 3*$SZT_NONAME_VALUES]
incr SZT_VALUES $SZT_NONAME_VALUES

puts $f "\#define SZL_STRINGS $SZL_STRINGS"
puts $f "\#define SZW_STRINGS $SZW_STRINGS"
puts $f ""
foreach {i} {
    STRINGS SCHEMES NAMES ALTS VALUES VALUE_TO TO_NAMES
    SCHEMES_INDEX SUBSCHEMES_INDEX INDEXES NONAME_VALUES
    VALUES_24 NONAME_VALUES_24
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
    puts $f "typedef $int IDX_$i\;"
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
        puts stderr "more that 64 bits in MSK_$i"
    }
    puts $f "typedef $int MSK_$i\;"
    puts $f ""
}
close $f
