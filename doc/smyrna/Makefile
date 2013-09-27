GRAPHFILES =  \
  figures/2D.png \
  figures/3D.png \
  figures/attr1.png \
  figures/attr2.png \
  figures/attr3.png \
  figures/attr4.png \
  figures/center.png \
  figures/details.png \
  figures/fisheye.png \
  figures/general.png \
  figures/graph.png \
  figures/gvpr.png \
  figures/labels.png \
  figures/magnifier.png \
  figures/no_fisheye.png \
  figures/nodelist.png \
  figures/pan.png \
  figures/smyrna.png \
  figures/topfish.png \
  figures/zoomin.png \
  figures/zoomout.png

TEXFILES = smyrna.tex attributes.tex controls.tex gui.tex \
  intro.tex using.tex

smyrna.pdf : smyrna.tex smyrna.bbl $(GRAPHFILES)
	pdflatex smyrna.tex
	@if grep "Label(s) may have changed" smyrna.log; then pdflatex smyrna.tex; fi

smyrna.bbl : smyrna.tex ../dotguide/graphdraw.bib smyrna.aux
	bibtex smyrna

smyrna.aux : smyrna.tex $(GRAPHFILES)
	pdflatex smyrna

clean: 
	rm -rf *.pdf *.log *.aux *.bbl *.blg *.log *.toc
