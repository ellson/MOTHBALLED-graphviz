#!/usr/bin/tclsh

source gv_doc_langs.tcl

set TEMPLATE gv_doc_template.tcl

set fn gv.i
set f [open $fn r]
set t [read $f [file size $fn]]
close $f

regsub -all {.*%inline %\{} $t {} t
regsub -all {%\}} $t {} t
regsub -all {extern} $t {} t
regsub -all {/\*.*?\*/} $t {} t
regsub -all {//.*?} $t {} t

foreach rec [split $t \n] {
	set rec [string trim $rec " \t;)"]
	if {[string length $rec] == 0} {continue}
	set i 0
	set params {}
	foreach type_name [split $rec "(,"] {
		set type_name [string trim $type_name]
		regsub -all {[ \t]+(\**)} $type_name {\1 } type_name
		foreach {type name} $type_name {break}
		if {! $i} {
			set func $name
			set functype $type
		} {
			lappend params $type $name
		}
		incr i
	}
	lappend FUNC($func) $functype $params
}

proc gv_doc_synopsis {} {
	global SYNOPSIS
	return [join $SYNOPSIS "\n.br\n"]
}

proc gv_doc_usage {} {
	global USAGE
	return [join $USAGE "\n.P\n"]
}

proc gv_doc_command {cmd} {
	global FUNC TYPES nameprefix paramstart paramsep paramend
	
	set res {}
	foreach {functype params} $FUNC($cmd) {
		set par {}
		foreach {paramtype param} $params {
			if {[info exists TYPES([list $paramtype $param])]} {
				lappend par $TYPES([list $paramtype $param])
			} {
				lappend par "$TYPES($paramtype) $param"
			}
		}
		if {[string length $TYPES($functype)]} {
			lappend res "$TYPES($functype) \\fB$nameprefix$cmd\\fR $paramstart[join $par $paramsep]$paramend" .br
		} {
			lappend res "\\fB$nameprefix$cmd\\fR $paramstart[join $par $paramsep]$paramend" .br
		}
			
	}
	return [join $res \n]
}

foreach lang [array names LANGS] {
	array set PROPS $LANGS($lang)
	array set TYPES $PROPS(TYPES)
	foreach {nameprefix paramstart paramsep paramend} $PROPS(SYNTAX) {break}
	set SYNOPSIS $PROPS(SYNOPSIS)
	set USAGE $PROPS(USAGE)
	set f [open gv_[set lang].man w]
	set ft [open $TEMPLATE r]
	puts $f [subst [read $ft [file size $TEMPLATE]]]
	close $ft
	close $f
}
