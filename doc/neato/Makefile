GRAPHFILES =  \
	ER.pdf \
	fancy.pdf \
	G4_lenwt.pdf \
	G4_orig.pdf \
	G4_pinned.pdf \
	inet.pdf \
	jho.pdf \
	process.pdf \
	typeshar.pdf

# note Tps not Tps2
# for unknown reasons, neato -Tps2  | epsftopdf | pdflatex is bad
%.pdf : %.dot
	neato -Gstart=867000 -Tps $< | epstopdf --filter --outfile=$@

neato.pdf : neato.tex neato.bbl $(GRAPHFILES)
	pdflatex neato.tex
	@if grep "Label(s) may have changed" neato.log; then pdflatex neato.tex; fi

neato.bbl : neato.tex ../dotguide/graphdraw.bib neato.aux
	bibtex neato

neato.aux : neato.tex $(GRAPHFILES)
	pdflatex neato

clean: 
	rm -rf *.ps *.pdf *.log 
