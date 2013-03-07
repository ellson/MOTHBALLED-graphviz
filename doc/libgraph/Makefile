TEX=latex
PTEX=pdflatex
DVI=dvips

all : Agraph.pdf

%.pdf : %.dot
	dot -Tps2 $< | epstopdf --filter --outfile=$@

Agraph.pdf : Agraph.tex sccmap.tex agmemread.tex
	$(PTEX) Agraph.tex
	@if grep "Label(s) may have changed" Agraph.log; then $(PTEX) Agraph.tex; fi

Agraph.ps : Agraph.dvi 
	@if grep "Label(s) may have changed" Agraph.log; then $(TEX) Agraph.tex; fi
	$(DVI) -o Agraph.ps Agraph
	
TEXS = graph.tex sccmap.tex agmemread.tex

sccmap.tex : sccmap.c
	lgrind -i -c -t 4 -l C sccmap.c > sccmap.tex

agmemread.tex : agmemread.c
	lgrind -i -c -t 4 -l C agmemread.c > agmemread.tex

Agraph.aux : Agraph.tex
	$(TEX) Agraph

clean: 
	rm -rf *.bbl *.aux *.dvi *.ps *.pdf *.log *.toc *.blg

