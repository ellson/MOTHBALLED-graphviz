TEX=latex
PTEX=pdflatex
DVI=dvips

all : libguide.pdf

%.pdf : %.dot
	dot -Tps2 $< | epstopdf --filter --outfile=$@

# libguide.bbl

libguide.pdf : libguide.tex
	$(PTEX) libguide
	bibtex libguide
	$(PTEX) libguide
	@if grep "Label(s) may have changed" libguide.log; then $(PTEX) libguide; fi

libguide.ps : libguide.dvi 
	@if grep "Label(s) may have changed" libguide.log; then $(TEX) libguide.tex; fi
	$(DVI) -o libguide.ps libguide
	
TEXS = libguide.tex intro.tex basic.tex layouts.tex drivers.tex codegen.tex \
       plugins.tex unconnect.tex build.tex samples.tex types.tex

libguide.dvi : libguide.bbl $(TEXS)
	$(TEX) libguide.tex
	@if grep "Label(s) may have changed" libguide.log; then $(TEX) libguide.tex; fi

libguide.bbl : libguide.tex graphdraw.bib libguide.aux
	bibtex libguide

libguide.aux : libguide.tex
	$(TEX) libguide

clean: 
	rm -rf *.bbl *.aux *.dvi *.ps *.pdf *.log *.toc *.blg

