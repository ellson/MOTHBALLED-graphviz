VERSION=0.1

CFLAGS=-O2 -Wall -DVERSION="$(VERSION)"
LDFLAGS = -lgd

diffimg: diffimg.o

GRAPH = "digraph G { hello -> world }"

test: diffimg
	echo $(GRAPH) | dot -Tpng:cg >hello1.png
	echo $(GRAPH) | dot -Tpng:cg >hello2.png
	./diffimg hello1.png hello2.png >test1.png
	echo $(GRAPH) | dot -Grankdir=LR -Tpng:cg >hello2.png
	./diffimg hello1.png hello2.png >test2.png

dist: clean
	mkdir diffimg-$(VERSION)
	tar cf - --exclude diffimg-$(VERSION) . | (cd diffimg-$(VERSION); tar xf -)
	tar czf - diffimg-$(VERSION) >diffimg-$(VERSION).tgz
	rm -rf diffimg-$(VERSION)

clean:
	rm -f diffimg *.o *.png *,tgz

