TEX=latex
PTEX=pdflatex
DVI=dvips

all : Agraph.pdf

%.pdf : %.dot
	dot -Tps2 $< | epstopdf --filter --outfile=$@

Agraph.pdf : Agraph.ps
	ps2pdf Agraph.ps Agraph.pdf

Agraph.ps : Agraph.dvi 
	@if grep "Label(s) may have changed" Agraph.log; then $(TEX) Agraph.tex; fi
	$(DVI) -o Agraph.ps Agraph
	
TEXS = graph.tex sccmap.tex

Agraph.aux : Agraph.tex
	$(TEX) Agraph

clean: 
	rm -rf *.bbl *.aux *.dvi *.ps *.pdf *.log *.toc *.blg

