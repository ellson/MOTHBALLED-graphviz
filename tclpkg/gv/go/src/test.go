package main
import ( "gv" )
func main() {
    var g    gv.Agraph_t
    var n, m gv.Agnode_t
//  var e    gv.Agedge_t
    g = gv.Digraph("G")
    n = gv.Node(g, "hello")
    m = gv.Node(g, "world")
        gv.Edge(n, m)
    gv.Write(g, "hello.out")
}
