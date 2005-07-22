#!/usr/bin/python

import gv

g = gv.digraph("G")
print gv.set(g,"aaa","xxx")
print gv.set(g,"aaa")
n = gv.node(g,"hello")
print gv.set(n,"label")
print gv.set(n,"aaa","xxx")
print gv.set(n,"aaa")
m = gv.node(g,"world")
print gv.set(m,"aaa")
e = gv.edge(n,m)
print gv.set(e,"aaa","xxx")
print gv.set(e,"aaa")
gv.rm(e)
gv.rm(m)
gv.rm(n)
gv.rm(g)

g = gv.stringgraph("digraph G {a->b}")
gv.rm(g)

g = gv.readgraph("../hello.dot")
gv.layout(g, "dot")
gv.writegraph(g, "hello.png", "png")
gv.rm(g)
