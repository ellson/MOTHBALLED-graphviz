// test.java

public class test {
    static {
	System.loadLibrary("gv");
    }

    public static void main(String[] args) {
	SWIGTYPE_p_Agraph_t g, sg;
	SWIGTYPE_p_Agnode_t n, m;
	SWIGTYPE_p_Agedge_t e;

	g = gv.digraph("G");
	System.out.println(gv.set(g,"aaa","xxx"));
	System.out.println(gv.set(g,"aaa"));
	sg = gv.graph(g,"SG");
	n = gv.node(g,"hello");
	System.out.println(gv.set(n,"label"));
	System.out.println(gv.set(n,"aaa","xxx"));
	System.out.println(gv.set(n,"aaa"));
	m = gv.node(g,"world");
        System.out.println(gv.set(m,"aaa"));
	e = gv.edge(n,m);
	System.out.println(gv.set(e,"aaa","xxx"));
        System.out.println(gv.set(e,"aaa"));
	gv.rm(e);
	gv.rm(n);
	gv.rm(m);
	gv.rm(g);

	g = gv.stringgraph("digraph G {a->b}");
	gv.rm(g);

	g = gv.readgraph("hello.dot");
	gv.layout(g,"dot");
	gv.writegraph(g,"hello.png","png");
	gv.rm(g);
    }
}
