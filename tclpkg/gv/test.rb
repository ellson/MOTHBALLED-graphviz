#!/usr/bin/ruby

require 'gv'

g = Gv.digraph("G")
print(g, "\n")
print(Gv::set(g, "aaa", "xxx"), "\n")
print(Gv::set(g, "aaa"), "\n")
n = Gv.node(g,"hello")
print(n, "\n")
print(Gv::set(n, "label"), "\n")
print(Gv::set(n, "aaa", "xxx"), "\n")
print(Gv::set(n, "aaa"), "\n")
m = Gv.node(g,"world")
print(m, "\n")
print(Gv::set(m, "aaa"), "\n")
e = Gv.edge(n,m)
print(e, "\n")
print(Gv::set(e, "aaa", "xxx"), "\n")
print(Gv::set(e, "aaa"), "\n")
Gv.rm(e)
Gv.rm(n)
Gv.rm(m)
Gv.rm(g)

g = Gv.stringgraph("digraph G {a->b}")
print(g, "\n")
Gv.rm(g)

g = Gv.readgraph("hello.dot")
print(g, "\n")
Gv.layout(g, "dot")
Gv.writegraph(g, "hello_ruby.png", "png")
Gv.rm(g)
