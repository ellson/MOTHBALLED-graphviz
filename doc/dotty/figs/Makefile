%.ps: %.xwd.gz
	gunzip < $< | xwdtopnm | ppmtopgm | pnmtops > $@

%.ps: %.ppm.gz
	gunzip < $< | ppmtopgm | pnmtops > $@

%.ps: %.xwd
	xwdtopnm < $<  | ppmtopgm | pnmtops > $@

%.ps: %.ppm
	ppmtopgm | pnmtops < $< > $@

%.ps: %.dot
	dot -Tps $< > $@

PSS = d1.ps d2.ps d3.ps d4.ps d5.ps d6.ps d7.ps d8.ps dotty.ps fa1.ps fa2.ps ldbx.ps

all: $(PSS)

clean:
	rm -rf *.ps
