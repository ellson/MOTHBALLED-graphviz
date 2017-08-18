#!/usr/bin/lua

---- importing ----
if string.sub(_VERSION,1,7)=='Lua 5.0' then
        -- lua5.0 does't have a nice way to do this
--        lib=loadlib('/usr/lib/graphviz/lua/libgv_lua.so','Gv_Init') 
        lib=loadlib('./libgv_lua.so','Gv_Init') 
        assert(lib)()
else
        -- lua 5.1 does
        require('gv')
end


g = gv.digraph("G")
print(gv.setv(g,"aaa","xxx"))
print(gv.getv(g,"aaa"))
n = gv.node(g,"hello")
print(gv.getv(n,"label"))
print(gv.setv(n,"aaa","xxx"))
print(gv.getv(n,"aaa"))
m = gv.node(g,"world")
print(gv.getv(m,"aaa"))
e = gv.edge(n,m)
print(gv.setv(e,"aaa","xxx"))
print(gv.getv(e,"aaa"))
gv.rm(e)
gv.rm(m)
gv.rm(n)
gv.rm(g)

g = gv.readstring("digraph G {a->b}")
gv.rm(g)

g = gv.read("hello.gv")
gv.layout(g, "dot")
gv.render(g, "png", "hello.png")
gv.rm(g)
