GO=$(GOROOT)/bin/go
LDFLAGS="-r `pwd`/gv -r `pwd`/example"

test: test.go prep
	$(GO) build -x -ldflags=$(LDFLAGS) test.go

.PHONY: prep
prep:
	mkdir -p gv
#	swig -c++ -go -intgosize 32 -o gv/gv_go.cpp ../../gv.i
#	sed -e 's/#include "gvc.h"/#include <graphviz\/gvc.h>/' <../../gv.cpp >gv/gv.cpp
#	sed -e 's/#include "gvc.h"/#include <graphviz\/gvc.h>/' <gv/gv_go.cpp >gv/gv_go.cpp2
#	mv gv/gv_go.cpp2 gv/gv_go.cpp
	sed -e 's/#include "gvc.h"/#include <graphviz\/gvc.h>/' <../../gv.i >gv/gv.swig
	sed -e 's/#include "gvc.h"/#include <graphviz\/gvc.h>/' \
		-e 's/DEMAND_LOADING/1/' <../../gv.cpp >gv/gv.cpp
	echo "package gv" >gv/gv.go

clean:
	rm -rf test gv */*.so
