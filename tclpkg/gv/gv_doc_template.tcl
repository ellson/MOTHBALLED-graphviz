.TH GV_[string toupper $lang] N \"[clock format [clock seconds] -format "%d %B %Y"]\"

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
.PP
Graph creation commands:

[gv_doc_command digraph]
[gv_doc_command graph]
[gv_doc_command strictdigraph]
[gv_doc_command strictgraph]
[gv_doc_command read]
[gv_doc_command readstring]
.PP
Node creation commands:

[gv_doc_command node]
.PP
Edge creation commands:

[gv_doc_command edge]
.PP
Object removal commands:

[gv_doc_command rm]
.PP
Attributes:

[gv_doc_command getv]

[gv_doc_command setv]
.PP
Iterators:

[gv_doc_command findattr]
[gv_doc_command firstedge]
[gv_doc_command nextedge]
[gv_doc_command firstin]
[gv_doc_command nextin]
[gv_doc_command firstout]
[gv_doc_command nextout]
[gv_doc_command firsthead]
[gv_doc_command nexthead]
[gv_doc_command firsttail]
[gv_doc_command nexttail]
[gv_doc_command firstnode]
[gv_doc_command nextnode]
[gv_doc_command firstsubg]
[gv_doc_command nextsubg]
[gv_doc_command firstsupg]
[gv_doc_command nextsupg]
[gv_doc_command firstattr]
[gv_doc_command nextattr]
[gv_doc_command ok]
.PP
Miscellaneous:

[gv_doc_command headof]
[gv_doc_command tailof]
[gv_doc_command nameof]
[gv_doc_command findedge]
[gv_doc_command findnode]
[gv_doc_command findsubg]
[gv_doc_command graphof]
[gv_doc_command rootof]
.PP
Layout:

[gv_doc_command layout]
.PP
Output:

[gv_doc_command render]

.SH BUGS

.SH ACKNOWLEDGEMENTS

.SH KEYWORDS

graph, dot, neato, fdp, circo, twopi, $lang.
