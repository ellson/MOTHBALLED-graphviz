LDFLAGS = -lgd

diffimg: diffimg.o

GRAPH = "digraph G { hello -> world }"

test: diffimg
	echo $(GRAPH) | dot -Tpng:cg >hello1.png
	echo $(GRAPH) | dot -Tpng:cg >hello2.png
	./diffimg hello1.png hello2.png >test1.png
	echo $(GRAPH) | dot -Grankdir=LR -Tpng:cg >hello2.png
	./diffimg hello1.png hello2.png >test2.png

clean:
	rm -f diffimg *.o *.png
