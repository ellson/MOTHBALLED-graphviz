#!/usr/bin/python

import gv

g = gv.digraph("G")
print g
print gv.set(g,"aaa","xxx")
print gv.set(g,"aaa")
n = gv.node(g,"hello")
print n
print gv.set(n,"label")
print gv.set(n,"aaa","xxx")
print gv.set(n,"aaa")
m = gv.node(g,"world")
print m
print gv.set(m,"aaa")
e = gv.edge(n,m)
print e
print gv.set(e,"aaa","xxx")
print gv.set(e,"aaa")
gv.rm(e)
gv.rm(m)
gv.rm(n)
gv.rm(g)

g = gv.stringgraph("digraph G {a->b}")
print g
gv.rm(g)

g = gv.readgraph("hello.dot")
print g
gv.layout(g, "dot")
gv.writegraph(g, "hello_python.png", "png")
gv.rm(g)
