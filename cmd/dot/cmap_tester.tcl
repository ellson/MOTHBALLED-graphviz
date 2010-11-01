#!/usr/bin/tclsh

# cmap tester - make visible the map areas from a .cmapx or .cmap file onto 
# a .png or .gif image file
#
# usage example:
#
#
# ./dot -Tpng -o example.png -Tcmapx -o example.cmapx < example.gv
# ./cmap_tester.tcl example | xv -


package require Gdtclft

if {! $argc} {
	puts stderr "Usage: $argv0 file"
	exit
}

set fn [file rootname [lindex $argv 0]]

if {[file exists $fn.gif]} {
	set image_type gif
} elseif {[file exists $fn.png]} {
	set image_type png
} elseif {[file exists [lindex $argv 0].png]} {
	set fn [lindex $argv 0]
	set image_type png
} elseif {[file exists [lindex $argv 0].gif]} {
	set fn [lindex $argv 0]
	set image_type gif
} else {
	puts stderr "image file $fn.gif or $fn.png does not exist."
	exit
}

if {[file exists $fn.cmap]} {
	set map_type cmap
} elseif {[file exists $fn.cmapx]} {
	set map_type cmapx
} else {
	puts stderr "map file $fn.cmap or $fn.cmapx does not exist."
	exit
}

set f [open $fn.$image_type r]
set gd [gd createFrom[string toupper $image_type] $f]
close $f

set turquoise [gd color resolve $gd 0 255 255]

set f [open $fn.$map_type r]
set map [read $f [file size $fn.$map_type]]
close $f

foreach {. coords} [regexp -all -inline {coords="([-0-9, ]*)"} $map] {
	set coords [split $coords ", "]
	if {[llength $coords] == 4} {
	    eval gd rectangle $gd $turquoise $coords
	} {
	    eval gd polygon $gd $turquoise $coords
	}
}

gd write[string toupper $image_type] $gd stdout
