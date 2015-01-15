SHAPEFILES = \
    Mcircle.pdf Mdiamond.pdf Mrecord.pdf Msquare.pdf \
    box.pdf circle.pdf diamond.pdf doublecircle.pdf \
    doubleoctagon.pdf egg.pdf ellipse.pdf hexagon.pdf \
    house.pdf invhouse.pdf invtrapezium.pdf invtriangle.pdf none.pdf \
    octagon.pdf parallelogram.pdf plaintext.pdf point.pdf \
    polygon.pdf record.pdf record.pdf trapezium.pdf \
    triangle.pdf tripleoctagon.pdf \

ARROWHEADFILES = \
    normal.pdf diamondarrow.pdf dot.pdf odot.pdf \
    inv.pdf invdot.pdf invodot.pdf \
    crow.pdf vee.pdf tee.pdf \
    boxarrow.pdf curve.pdf icurve.pdf \
    no_arrow.pdf diamond.pdf

GRAPHFILES =  \
        asde91.pdf \
        clust1.pdf \
        clust2.pdf \
	compound.pdf \
        fsm.pdf \
        graph1.pdf \
        graph2.pdf \
        html.pdf \
        hashtable.pdf \
        poly.pdf \
	recordex.pdf \
        smlred.pdf \
        structs.pdf \
        tree.pdf \
	$(SHAPEFILES) $(ARROWHEADFILES)

# The following used to work, but it broke when graphviz -Tps went to
# BoundingBox: (atend) as the epstopdf script explicitly says it does
# not handle this case!  Let's hope that we have native -Tpdf in dot.
%.pdf : %.dot
	dot -Tps2 $< -o $<.ps; ps2pdf $<.ps $@

dotguide.pdf : dotguide.tex dotguide.bbl $(GRAPHFILES)
	pdflatex dotguide.tex
	@if grep "Label(s) may have changed" dotguide.log; then pdflatex dotguide.tex; fi

dotguide.bbl : dotguide.tex ../dotguide/graphdraw.bib dotguide.aux
	bibtex dotguide

dotguide.aux : dotguide.tex $(GRAPHFILES)
	pdflatex dotguide

clean: 
	rm -rf *.ps *.pdf *.log  dotguide.aux dotguide.blg dotguide.bbl

