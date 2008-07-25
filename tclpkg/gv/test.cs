// test.cs

using System;

public class test {
    static void Main() {
	SWIGTYPE_p_Agraph_t g, sg;
	SWIGTYPE_p_Agnode_t n, m;
	SWIGTYPE_p_Agedge_t e;

	g = gv.digraph("G");
	Console.WriteLine(gv.setv(g,"aaa","xxx"));
	Console.WriteLine(gv.getv(g,"aaa"));
	sg = gv.graph(g,"SG");
	n = gv.node(g,"hello");
	Console.WriteLine(gv.getv(n,"label"));
	Console.WriteLine(gv.setv(n,"aaa","xxx"));
	Console.WriteLine(gv.getv(n,"aaa"));
	m = gv.node(g,"world");
        Console.WriteLine(gv.getv(m,"aaa"));
	e = gv.edge(n,m);
	Console.WriteLine(gv.setv(e,"aaa","xxx"));
        Console.WriteLine(gv.getv(e,"aaa"));
	gv.rm(e);
	gv.rm(n);
	gv.rm(m);
	gv.rm(g);

	g = gv.readstring("digraph G {a->b}");
	gv.rm(g);

	g = gv.read("hello.gv");
	gv.layout(g,"dot");
	gv.render(g,"png", "hello.png");
	gv.rm(g);
    }
}
