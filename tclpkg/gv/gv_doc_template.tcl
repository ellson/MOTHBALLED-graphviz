.TH gv 3[string tolower $lang] \"[
    set buildtime [clock seconds]
    if { [info exists ::env(SOURCE_DATE_EPOCH) ] } {
         set buildtime $::env(SOURCE_DATE_EPOCH)
    }
    clock format $buildtime -format "%d %B %Y"]\"

.SH NAME

gv_$lang \- graph manipulation in $lang

.SH SYNOPSIS

[gv_doc_synopsis]

.SH USAGE

[gv_doc_usage]

.SH INTRODUCTION

.B gv_$lang
is a dynamically loaded extension for
.B $lang
that provides access to the graph facilities of
.B graphviz.

.SH COMMANDS

[gv_doc_commands]

.SH KEYWORDS

graph, dot, neato, fdp, circo, twopi, $lang.
