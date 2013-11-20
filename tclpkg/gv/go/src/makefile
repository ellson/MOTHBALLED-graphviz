GO=$(GOROOT)/bin/go

test: test.go prep
	$(GO) build -work -gccgoflags '-lgvc' -compiler gccgo test.go

.PHONY: prep
prep:
	mkdir -p gv
	sed -e 's/#include "gvc.h"/#include <graphviz\/gvc.h>/' <../../gv.i >gv/gv.swigcxx
	sed -e 's/#include <gvc.h>/#include <graphviz\/gvc.h>/' <../../gv_dummy_init.c >gv/gv_dummy_init.c
	sed -e 's/#include "gvplugin.h"/#include <graphviz\/gvplugin.h>/' <../../gv_builtins.c >gv/gv_builtins.c
	sed -e 's/#include "gvc.h"/#include <graphviz\/gvc.h>/' \
		-e 's/DEMAND_LOADING/1/' <../../gv.cpp >gv/gv.cpp
	echo "package gv" >gv/gv.go

clean:
	rm -rf test gv /tmp/go-build*
