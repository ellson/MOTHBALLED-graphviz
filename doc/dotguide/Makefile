SHAPEFILES = \
    Mcircle.pdf Mdiamond.pdf Mrecord.pdf Msquare.pdf \
    box.pdf circle.pdf diamond.pdf doublecircle.pdf \
    doubleoctagon.pdf egg.pdf ellipse.pdf hexagon.pdf \
    house.pdf invhouse.pdf invtrapezium.pdf invtriangle.pdf \
    octagon.pdf parallelogram.pdf plaintext.pdf point.pdf \
    polygon.pdf record.pdf record.pdf trapezium.pdf \
    triangle.pdf tripleoctagon.pdf \

ARROWHEADFILES = \
    normal.pdf dot.pdf odot.pdf \
    inv.pdf invdot.pdf invodot.pdf \
    none.pdf

GRAPHFILES =  \
        asde91.pdf \
        clust1.pdf \
        clust2.pdf \
	compound.pdf \
        fsm.pdf \
        graph1.pdf \
        graph2.pdf \
        hashtable.pdf \
        poly.pdf \
	recordex.pdf \
        smlred.pdf \
        structs.pdf \
        tree.pdf \
	$(SHAPEFILES) $(ARROWHEADFILES)

%.pdf : %.dot
	dot -Tps $< | epstopdf --filter --outfile=$@

dotguide.pdf : dotguide.tex dotguide.bbl $(GRAPHFILES)
	pdflatex dotguide.tex
	@if grep "Label(s) may have changed" dotguide.log; then pdflatex dotguide.tex; fi

dotguide.bbl : dotguide.tex ../dotguide/graphdraw.bib dotguide.aux
	bibtex dotguide

dotguide.aux : dotguide.tex $(GRAPHFILES)
	pdflatex dotguide

clean: 
	rm -rf *.ps *.pdf *.log 

