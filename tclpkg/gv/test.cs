// test.cs

using System;

public class test {
    static void Main() {
	SWIGTYPE_p_Agraph_t g, sg;
	SWIGTYPE_p_Agnode_t n, m;
	SWIGTYPE_p_Agedge_t e;

	g = gv.digraph("G");
	Console.WriteLine(gv.set(g,"aaa","xxx"));
	Console.WriteLine(gv.set(g,"aaa"));
	sg = gv.graph(g,"SG");
	n = gv.node(g,"hello");
	Console.WriteLine(gv.set(n,"label"));
	Console.WriteLine(gv.set(n,"aaa","xxx"));
	Console.WriteLine(gv.set(n,"aaa"));
	m = gv.node(g,"world");
        Console.WriteLine(gv.set(m,"aaa"));
	e = gv.edge(n,m);
	Console.WriteLine(gv.set(e,"aaa","xxx"));
        Console.WriteLine(gv.set(e,"aaa"));
	gv.rm(e);
	gv.rm(n);
	gv.rm(m);
	gv.rm(g);

	g = gv.stringgraph("digraph G {a->b}");
	gv.rm(g);

	g = gv.readgraph("hello.dot");
	gv.layout(g,"dot");
	gv.render(g,"png", "hello.png");
	gv.rm(g);
    }
}
