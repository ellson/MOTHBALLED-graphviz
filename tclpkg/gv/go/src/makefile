GO=$(GOROOT)/bin/go

test: test.go prep
	$(GO) build -x -work -ldflags='-extldflags=/usr/lib64/libgvc.so' test.go

.PHONY: prep
prep:
	mkdir -p gv
	sed -e 's/#include "gvc.h"/#include <graphviz\/gvc.h>/' <../../gv.i >gv/gv.swigcxx
	sed -e 's/#include "gvc.h"/#include <graphviz\/gvc.h>/' \
		-e 's/DEMAND_LOADING/1/' <../../gv.cpp >gv/gv.cpp
	echo "package gv" >gv/gv.go

clean:
	rm -rf test gv
