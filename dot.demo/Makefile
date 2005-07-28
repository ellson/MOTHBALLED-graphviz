CFLAGS=`pkg-config libgvc --cflags` -Wall -g -O2
LDFLAGS=`pkg-config libgvc --libs`

all: dot demo simple

dot: dot.o builtins.o

demo: demo.o builtins.o

simple: simple.o builtins.o

clean:
	rm -rf *.o dot demo simple *.png
