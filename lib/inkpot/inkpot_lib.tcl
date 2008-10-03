#!/usr/bin/tclsh

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
				set ALL_SCHEMES($scheme) {}
				lappend ALL_VALUES($value) $coding $scheme $color
			}
			2 {
				foreach {subscheme index} $color {break}
				lappend ALL_STRINGS($subscheme) subscheme
				lappend ALL_INDEX_SCHEMES([list $scheme $subscheme 0]) $index $value
				lappend ALL_INDEXES($value) $coding $scheme $subscheme 0 $index
			}
			3 {
				foreach {subscheme range index} $color {break}
				lappend ALL_STRINGS($subscheme) subscheme
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
foreach {value} [array names ALL_VALUES] {
	foreach {coding scheme color} $ALL_VALUES($value) {
		lappend ALL_ALTS([list $color $value]) $scheme
	}
}
foreach {color_value} [lsort -ascii [array names ALL_ALTS]] {
	foreach {color value} $color_value {break}

	# give preference to x11 scheme names, and hope that someone doesn't invent a z11 scheme
	set schemeset [lsort -ascii -decreasing $ALL_ALTS($color_value)]

	lappend ALL_ALTSETS($color) $value $schemeset
	lappend ALL_MAPSETS($value) $color $schemeset
}
foreach {color} [lsort -ascii [array names ALL_ALTSETS]] {
	set altset $ALL_ALTSETS($color)
	lappend ALL_ALTSET_COLORS($altset) $color
	foreach {value schemeset} $altset {
		lappend ALL_VALUE_ALTSETS($value) $altset
	}
}
foreach {value} [lsort -ascii [array names ALL_MAPSETS]] {
	set mapset $ALL_MAPSETS($value)
	lappend ALL_MAPSET_VALUES($mapset) $value
	foreach {color schemeset} $mapset {
		lappend ALL_COLOR_MAPSETS($color) $mapset
	}
}
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

# some support procs

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


# degug support
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
	
#------------------------------------------------- write inkpot_value_table.h
set f [open inkpot_value_table.h w]
puts $f $preamble

# generate TAB_VALUES_24
set SZT_VALUES 0
tab_begin $f "unsigned char TAB_VALUES_24\[SZT_VALUES_24\] = {"
foreach {value} [lsort -dictionary [array names ALL_VALUES]] {
	tab_begin_block $f $SZT_VALUES

	foreach {r g b} $value {break}
	tab_elem $f [format "0x%02x,0x%02x,0x%02x," $r $g $b]

	set ALL_VALUES_coded($value) $SZT_VALUES
	incr SZT_VALUES
	
	tab_end_block $f $ALL_MAPSETS($value)
}
tab_end $f "};\n"

# generate NONAME_VALUES_24
set SZT_NONAME_VALUES 0
tab_begin $f "unsigned char TAB_NONAME_VALUES_24\[SZT_NONAME_VALUES_24\] = {"
foreach {value} [lsort -dictionary [array names ALL_INDEXES]] {
	if {! [info exists ALL_VALUES($value)]} {
		tab_begin_block $f $SZT_NONAME_VALUES
	
		foreach {r g b} $value {break}
		tab_elem $f [format "0x%02x,0x%02x,0x%02x," $r $g $b]

		set ALL_VALUES_coded($value) [expr $SZT_NONAME_VALUES + $SZT_VALUES]
		incr SZT_NONAME_VALUES

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
	
#	tab_elem $f "{$ALL_SCHEME_STRINGS_coded($scheme),$ALL_INDEX_SCHEMES_coded($scheme)},"
	tab_elem $f "{$ALL_SCHEME_STRINGS_coded($scheme)},"
	
	tab_end_block $f $scheme
	
	set ALL_SCHEMES($scheme) [list $SZT_SCHEMES [expr 1 << $SZT_SCHEMES]]
	incr SZT_SCHEMES
}
tab_end $f "};\n"
	
	
# generate TAB_ALTS
set SZT_ALTS 0
tab_begin $f "inkpot_name_t TAB_ALTS\[SZT_ALTS\] = {"
foreach {color} [lsort -ascii [array names ALL_ALTSETS]] {
	set cnt 0
	set altset $ALL_ALTSETS($color)
	set aliases [lsort -ascii $ALL_ALTSET_COLORS($altset)]
	foreach {value schemeset} $altset {
		if {[info exists alts($value)]} {
			puts stderr "something weird going on"
		}
		lappend alts($value) $schemeset $aliases
		incr cnt
	}
	foreach {value} [lsort -ascii [array names alts]] {
		set scheme_bits 0
		foreach {schemeset aliases} $alts($value) {break}
		foreach {scheme} $schemeset {
			foreach {scheme_idx scheme_bit} $ALL_SCHEMES($scheme) {break}
			set scheme_bits [expr $scheme_bits | $scheme_bit]
		}
		lappend value_schemebits $value $scheme_bits $aliases
	}
	
	set isneeded 0
	if {$cnt == 0} {
		puts stderr "shouldn't happen - zero alts: $color"
	} elseif {$cnt == 1} {
		foreach {value scheme_bits} $value_schemebits {break}
		set ALL_ALTSETS_coded($color) "$ALL_VALUES_coded($value),[format {0x%x} $scheme_bits]"
		# don't need entry in TAB_ALTS for this case
	} else {
		set first_idx $SZT_ALTS
		foreach {value scheme_bits aliases} $value_schemebits {
			set firstcolor [lindex $aliases 0] 
			if {[string equal $color $firstcolor]} {
				tab_begin_block $f $first_idx
				incr isneeded
				
				tab_elem $f "{[incr cnt -1],$ALL_VALUES_coded($value),[format {0x%x} $scheme_bits]},"
				set ALL_ALTSETS_coded($color) "$SZT_ALTS,0"
				incr SZT_ALTS
			} {
				# the sorting means that this value has already been saved
				set ALL_ALTSETS_coded($color) $ALL_ALTSETS_coded($firstcolor)
				# don't need entry in TAB_ALTS for this case
			}
		}
	}
	if {$isneeded} {tab_end_block $f $aliases}
	
	array unset alts
	unset value_schemebits
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
