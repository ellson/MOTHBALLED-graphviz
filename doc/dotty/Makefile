%.dvi: %.tex
	while [[ ! -f $@ ]] do
		latex $<
#		chkllog $(%)
	done

%.ps: %.dvi
	dvips $<.dvi

dotty.bbl:
	latex dotty.tex
	bibtex dotty
	rm dotty.dvi

all: dotty.bbl dotty.dvi

clean:
	rm -f *.aux *.log *.bbl *.blg *.ps *.dvi
