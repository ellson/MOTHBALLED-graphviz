SRCS = \
       src/book.xml \
       src/bookinfo.xml \
       src/graphviz_plugins.xml \
       src/graphviz_scripting.xml

all: html pdf

dvi fo javahelp man pdf ps txt xhtml xhtml-nochunks: $(SRCS)
	xmlto $@ src/book.xml


html html-nochunks htmlhelp: $(SRCS)
	xmlto $@ -o $@ src/book.xml
	cp -rp src/imgs $@/

clean:
	rm -rf html* book.*
