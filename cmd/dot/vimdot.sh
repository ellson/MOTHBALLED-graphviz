#!/bin/sh

# Written by: John Ellson <ellson@research.att.com>

if test -z $1; then f=noname.dot; else f=$1; fi
if ! test -f $f; then cat >$f <<EOF
digraph noname {
	graph [layout=dot rankdir=LR]

// This is just an example for you to use as a template.
// Edit as you like. Whenever you save a legal graph,
// the graph layout in dot's window will be updated.

	{vim dot} -> vimdot
}
EOF
fi
if ! test -w $f; then echo "warning: $f is not writable";fi

# dot -Txlib watches the file $f for changes using inotify()
dot -Txlib $f &
# open an editor in the file (coudl be any editor)
gvim $f &
