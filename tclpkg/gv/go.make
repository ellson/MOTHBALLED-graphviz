GO=$(GOROOT)/bin/go

test: test.go prep
	( cd go/src; \
	$(GO) build -x -work -gccgoflags '-lgvc' -compiler gccgo test.go && mv test ../../ )

.PHONY: prep
prep:
	mkdir -p go/src/gv
	sed -e 's/#include "gvc.h"/#include <graphviz\/gvc.h>/' <gv.i >go/src/gv/gv.swigcxx
	sed -e 's/#include <gvc.h>/#include <graphviz\/gvc.h>/' <gv_dummy_init.c >go/src/gv/gv_dummy_init.c
	sed -e 's/#include "gvplugin.h"/#include <graphviz\/gvplugin.h>/' <gv_builtins.c >go/src/gv/gv_builtins.c
	sed -e 's/#include "gvc.h"/#include <graphviz\/gvc.h>/' \
		-e 's/DEMAND_LOADING/1/' <gv.cpp >go/src/gv/gv.cpp
	echo "package gv" >go/src/gv/gv.go
	cat test.go >go/src/test.go

clean:
	rm -rf test go /tmp/go-build*
