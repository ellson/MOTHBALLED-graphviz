#!/usr/bin/tclsh

proc find_initial fn {
	set author unknown
	set date 2011

	set rxp {revision 1.1[^.0-9]*date: ([0-9]+).*?author: ([A-Za-z0-9]+);}
	if {! [catch "exec cvs log $fn" res]} {
		regexp $rxp $res . date author
	}
	return [list $date $author]
}

proc dofile fn {
	set rxp {/\*\*\*\*.*?Copyright[^-0-9]*?([-0-9]+).*?Common Public License.*?\*\*\*\*/}

	set epl_header {/*************************************************************************
 * Copyright (c) $epl_date $epl_initial_copyright_owner $epl_other_copyright_owners
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors: See CVS logs. Details at http://www.graphviz.org/
 *************************************************************************/}

	set epl_date "2011"
	set epl_initial_copyright_owner "AT&T Intellectual Property"
	set epl_other_copyright_owners {}
	set epl_initial_author {unknown}

	set f [open $fn r]
	set t [read $f [file size $fn]]
	close $f

	if {[regexp -indices $rxp $t cpl cpl_date]} { 
#		puts "$fn $cpl $cpl_date [string length $t]"

		foreach {cpl_first cpl_last} $cpl {break}
#		puts [string range $t $cpl_first $cpl_last]

#		foreach {cpl_date_first cpl_date_last} $cpl_date {break}
#		puts [string range $t $cpl_date_first $cpl_date_last]

#		puts [find_initial $fn]

#		set epl_date [string range $t $cpl_date_first $cpl_date_last]

		set f [open $fn w]
		puts -nonewline $f [string range $t 0 $cpl_first-1]
		puts -nonewline $f [subst $epl_header]
		puts -nonewline $f [string range $t $cpl_last+1 end]
		close $f
	}
}

proc doname n {
	if [file isdirectory $n] {
		foreach gn [glob -nocomplain $n/*] {
			doname $gn
		}

	} {
		dofile $n
	}
}

foreach n $argv {
	doname $n
}
