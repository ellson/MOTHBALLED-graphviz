FORMATS="\
	-Tpng:cairo:cairo \
	-Tpng:cairo:gdk \
	-Tpng:cairo:devil \
	-Tjpg:gd:gd \
	-Tjpg:cairo:gdk \
	-Tgif:gd:gd \
	-Tgif:cairo:gd \
	-Tsvg:svg:core \
	-Tsvg:cairo:cairo \
	-Tpdf:cairo:cairo \
	-Tico:cairo:gdk"

echo "digraph { ranksep=2; hello -> world }" >hello.gv

dot hello.gv -v -Gmargin="0" $FORMATS -O

(
	echo "digraph { rankdir=LR; node [shape=box]"
	edge=""
	chain=""
	for f in $FORMATS; do
		f=${f#-T}
		a=${f%%:*}
		b=${f#*:}
		c=${b%:*}
		b=${b#*:}
		f=${b}.${c}.${a}
		echo "  \"$f\" [image=\"hello.gv.$f\" label=\"${a}\\l${b}\\l${c}\\l\"]"
		chain="\"$f\"${edge}$chain"
		edge=" -> "
	done
	echo "  $chain"
	echo "}"
) >ab.gv
		
dot ab.gv    -v -Gmargin="0" $FORMATS -O
