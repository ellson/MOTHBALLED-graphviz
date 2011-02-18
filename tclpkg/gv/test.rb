#!/usr/bin/ruby

require './gv'

g = Gv.digraph("G")
print(Gv.setv(g, "aaa", "xxx"), "\n")
print(Gv.getv(g, "aaa"), "\n")
n = Gv.node(g,"hello")
print(Gv.getv(n, "label"), "\n")
print(Gv.setv(n, "aaa", "xxx"), "\n")
print(Gv.getv(n, "aaa"), "\n")
m = Gv.node(g,"world")
print(Gv.getv(m, "aaa"), "\n")
e = Gv.edge(n,m)
print(Gv.setv(e, "aaa", "xxx"), "\n")
print(Gv.getv(e, "aaa"), "\n")
Gv.rm(e)
Gv.rm(n)
Gv.rm(m)
Gv.rm(g)

g = Gv.readstring("digraph G {a->b}")
Gv.rm(g)

g = Gv.read("hello.gv")
Gv.layout(g, "dot")
Gv.render(g, "png", "hello.png")
Gv.rm(g)
