%.dvi: %.tex
	latex $<

%.ps :  %.dvi
	dvips $< -o $@

%.ps : %.dot
	dot -Tps $< -o $@

all: lefty.bbl lefty.dvi

lefty.bbl:
	latex lefty.tex
	bibtex lefty
	rm lefty.dvi

clean:
	rm -f *.aux *.log *.bbl *.blg *.ps *.dvi
