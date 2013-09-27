%.ps: %.xwd.gz
	gunzip < $< | xwdtops -bw > $@

%.ps: %.ppm.gz
	gunzip < $< | ppmtopgm | pnmtops > $@

%.ps: %.xwd
	xwdtops -bw < $< > $@

%.ps: %.ppm
	pnmtops -bw < $< > $@

%.ps: %.dot
	dot -Tps $< > $@

%.ps: %.lefty
	lefty -x -el 2 -ps $@ $<

PSS = fig2tpa.ps fig2tpb.ps figboxinitial.ps figboxfinal.ps figfractalonea.ps figfractaloneb.ps figfractalmva.ps figfractalmvb.ps figfractaltracea.ps figfractaltraceb.ps figfractaltracec.ps figradixtree.ps figbigtree.ps figtria.ps figtrib.ps figdag.ps lefty.ps

all: $(PSS)

clean:
	rm -rf *.ps
