#!/usr/bin/tclsh

set fn /usr/lib64/graphviz/config5
set f [open $fn r]
set d [read $f [file size $fn]]
close $f

regsub -all -line {^#.*$} $d {} d

proc mergealias {format} {
    switch $format {
	jpg - jpeg - jpe {return jpg/jpe/jpeg}
	default {return $format}
    }
}

foreach {lib name contents} $d {
    set LIB($name) {}
    foreach {type plugins} $contents {
	foreach {plugin quality} $plugins {
	    switch $type {
		render {
		    lappend LIB_RENDER($name) $plugin
		}
		loadimage {
		    foreach {iformat irender} [split $plugin :] {break}
		    set iformat [mergealias $iformat]
		    lappend IFORMAT($iformat) {}
		    lappend INAMEFORMAT([list $name $iformat]) $irender
		    lappend LIB_IFORMAT($name) $iformat
		}
		device {
		    foreach {oformat orender} [split $plugin :] {break}
		    set oformat [mergealias $oformat]
		    lappend OFORMAT($oformat) {}
		    lappend ONAMEFORMAT([list $name $oformat]) $orender
		    lappend LIB_OFORMAT($name) $oformat
		}
	    }
	}
    }
}

#set f [open plugins.gv w]
set f stdout
puts $f "digraph G {"
puts $f "\trankdir=LR ranksep=2"
puts $f "\tsubgraph { rank=same"
foreach {iformat} [lsort [array names IFORMAT]] {
    puts "\t\t\"i-$iformat\" \[label=\"$iformat\"\]"
}
puts $f "\t}"
puts $f "\tsubgraph { rank=same"
foreach {oformat} [lsort [array names OFORMAT]] {
    puts "\t\t\"o-$oformat\" \[label=\"$oformat\"\]"
}
puts $f "\t}"
foreach {name} [array names LIB] {
    puts $f "\tsubgraph cluster_$name { label=\"$name\" penwidth=2"
    if {[info exists LIB_IFORMAT($name)]} {
        puts $f "\t\tsubgraph { rank=same"
        foreach {iformat} [lsort -unique $LIB_IFORMAT($name)] {
            puts $f "\t\t\t\"i-$name-$iformat\" \[label=\"$iformat\"\]"
        }
        puts $f "\t\t}"
    }
    if {[info exists LIB_RENDER($name)]} {
        puts $f "\t\tsubgraph { rank=same"
        foreach {render} [lsort $LIB_RENDER($name)] {
            puts $f "\t\t\t\"r-$render\" \[label=\"$render\"\]"
        }
        puts $f "\t\t}"
    }
    if {[info exists LIB_OFORMAT($name)]} {
        puts $f "\t\tsubgraph { rank=same"
        foreach {oformat} [lsort -unique $LIB_OFORMAT($name)] {
            puts $f "\t\t\t\"o-$name-$oformat\" \[label=\"$oformat\"\]"
        }
        puts $f "\t\t}"
    }
    puts $f "\t}"
}
foreach {name_iformat} [lsort [array names INAMEFORMAT]] {
    foreach {name iformat} $name_iformat {break}
    if {[string equal $iformat svg]} {set color red} {set color black}
    puts $f "\t\"i-$iformat\" -> \"i-$name-$iformat\" \[color=$color\]"
    foreach {irender} [lsort -unique $INAMEFORMAT($name_iformat)] {
	puts $f "\t\"i-$name-$iformat\" -> \"r-$irender\" \[color=$color\]"
	set RENDERCOLOR($irender) $color
    }
}
foreach {name_oformat} [lsort [array names ONAMEFORMAT]] {
    foreach {name oformat} $name_oformat {break}
    foreach {orender} [lsort -unique $ONAMEFORMAT($name_oformat)] {
	if {[info exists RENDERCOLOR($orender)]} {
	    set color $RENDERCOLOR($orender)
        } {
	    set color black
        }
	puts $f "\t\"r-$orender\" -> \"o-$name-$oformat\" -> \"o-$oformat\" \[color=$color\]"
    }
}
puts $f "}"
#close $f
