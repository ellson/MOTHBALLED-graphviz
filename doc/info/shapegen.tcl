#!/usr/bin/tclsh

set shapes {
	box {}
	polygon {sides=7 label="" style=filled}
	ellipse {}
	circle {}
	point {}
	egg {}
	triangle {}
	plaintext { }
	diamond {}
	trapezium {}
	parallelogram {}
	house {}
	pentagon {}
	hexagon {}
	septagon {}
	octagon {}
	doublecircle {}
	doubleoctagon {}
	tripleoctagon {}
	invtriangle {}
	invtrapezium {}
	invhouse {}
	Mdiamond {}
	Msquare {}
	Mcircle {}
	rect {}
	rectangle {}
	none { }
	note {}
	tab {}
	folder {}
	box3d {}
	component {}
}

set rowsize 4
set h [open temp.html w]
set count 0
puts $h "<TABLE ALIGN=\"CENTER\">"
foreach {shape attrs} $shapes {
	if {$count % $rowsize == 0} {
		if {$count != 0} {
			puts $h "  </TR>"
		}
		puts $h "  <TR ALIGN=\"CENTER\">"
	}
	puts $h "    <TD><IMG SRC=\"$shape.gif\"><BR><A NAME=\"d:$shape\">$shape</A></TD>"
	incr count

	set f [open $shape.dot w]
	puts $f "digraph G \{"
	puts $f "graph \[pad=\".15\"\]"
	if {[string length $attrs]} {
		puts $f "$shape \[shape=$shape $attrs\]"
	} { 	
		puts $f "$shape \[label=\"\" shape=$shape style=filled\]"
	}
	puts $f "\}"
	close $f
	exec dot -Tgif $shape.dot -o $shape.gif

	file delete $shape.dot
}
puts $h "  </TR>"
puts $h "</TABLE>"
close $h
