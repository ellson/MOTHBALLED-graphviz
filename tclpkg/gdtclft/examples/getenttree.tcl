#!/bin/sh
# next line is a comment in tcl \
exec tclsh "$0" ${1+"$@"}

# get names for  html-4.0 characters from: 
#          http://www.w3.org/TR/REC-html40/sgml/entities.html
set f [open entities.html r]
while {! [eof $f]} {
	set rec [gets $f]
	if {[scan $rec {&lt;!ENTITY %s CDATA &quot;&amp;#%d;&quot; --} name val] == 2} {
		set entity($name) $val
	}
}

set depth 0
set previous {}
set previousentity {}
foreach n [lsort [array names entity]] {
	set current [split $n {}]
	set i 0
	while {[lindex $previous $i] == [lindex $current $i]} {incr i}
	if {$previous != {}} {
		set prefix [format %[set depth]s {}]
		if {$i > $depth} {
			puts "$prefix[join [lrange $previous $depth [expr $i -1]] {}]"
			puts "$prefix [join [lrange $previous $i end] {}] $previousentity"
		} elseif {$i == $depth} {
			puts "$prefix[join [lrange $previous $i end] {}] $previousentity"
		} {
			puts "$prefix[join [lrange $previous $depth end] {}] $previousentity"
		}
	}
	set previousentity $entity($n)
	set depth $i
	set previous $current
}
puts "[format %[set depth]s {}][join [lrange $previous $depth end] {}] $entity($n)"
