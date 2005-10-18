COMPILE=libtool --tag=CC --mode=compile ${CC} -c
LINK=libtool --tag=CC --mode=link ${CC}

CFLAGS=`pkg-config libgvc --cflags` -Wall -g -O2
LDFLAGS=`pkg-config libgvc --libs` -export-dynamic -dlpreopen self -export-symbols-regex '.*_LTX_library'

all: dot demo simple

dot: dot.lo builtins.lo
	${LINK} ${LDFLAGS} -o $@ dot.lo builtins.lo

dot.lo: dot.c
	${COMPILE} ${CFLAGS} -o $@ dot.c

demo: demo.lo builtins.lo
	${LINK} ${LDFLAGS} -o $@ demo.lo builtins.lo

demo.lo: demo.c
	${COMPILE} ${CFLAGS} -o $@ demo.c

simple: simple.lo builtins.lo
	${LINK} ${LDFLAGS} -o $@ simple.lo builtins.lo

simple.lo: simple.c
	${COMPILE} ${CFLAGS} -o $@ simple.c

builtins.lo: builtins.c
	${COMPILE} ${CFLAGS} -o $@ builtins.c

clean:
	rm -rf *.o *.lo .libs dot demo simple *.png
