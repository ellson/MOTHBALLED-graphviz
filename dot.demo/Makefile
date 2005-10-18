COMPILE=libtool --tag=CC --mode=compile ${CC} -c
LINK=libtool --tag=CC --mode=link ${CC}

CFLAGS=`pkg-config libgvc --cflags` -Wall -g -O2
LDFLAGS=`pkg-config libgvc --libs` -export-dynamic -dlpreopen self -export-symbols-regex '.*_LTX_library'

all: dot demo simple

dot: dot.lo
	${LINK} ${LDFLAGS} -o $@ dot.lo

dot.lo: dot.c
	${COMPILE} ${CFLAGS} -o $@ dot.c

demo: demo.lo
	${LINK} ${LDFLAGS} -o $@ demo.lo

demo.lo: demo.c
	${COMPILE} ${CFLAGS} -o $@ demo.c

simple: simple.lo
	${LINK} ${LDFLAGS} -o $@ simple.lo

simple.lo: simple.c
	${COMPILE} ${CFLAGS} -o $@ simple.c

clean:
	rm -rf *.o *.lo .libs dot demo simple *.png
