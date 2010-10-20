GLADE_CFLAGS = -I/usr/local/include/cairo -I/usr/local/include/pixman-1 -I/usr/include/libglade-2.0 -I/usr/include/gtk-2.0 -I/usr/include/libxml2 -I/usr/lib64/gtk-2.0/include -I/usr/include/atk-1.0 -I/usr/include/pango-1.0 -I/usr/include/glib-2.0 -I/usr/lib64/glib-2.0/include -I/usr/include/freetype2 -I/usr/include/libpng12  

GLADE_LIBS = -L/usr/local/lib -L/lib64 -lglade-2.0 -lgtk-x11-2.0 -lxml2 -lz -lgdk-x11-2.0 -latk-1.0 -lgdk_pixbuf-2.0 -lm -lpangocairo-1.0 -lpango-1.0 -lcairo -lgobject-2.0 -lgmodule-2.0 -ldl -lglib-2.0  

CC = gcc
CPPFLAGS =  -I/usr/local/include
CFLAGS = -g -O2 -Wno-unknown-pragmas -Wstrict-prototypes -Wpointer-arith -Wall -ffast-math
PROJ = cairo_test

%.o : %.c
	$(CC) $(CFLAGS) $(GLADE_CFLAGS) $(CPPFLAGS) $(GLADE_LIBS) $< -o $(*F)





