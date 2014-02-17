// test.java

/*  On OSX:
 * javac -classpath $HOME/lib/graphviz/java test.java
 * java -classpath .:$HOME/lib/graphviz/java -Djava.library.path="$HOME/lib/graphviz/java/org/graphviz"  test
 */

import org.graphviz.*;

public class test {
    static {
	System.loadLibrary("gv");
    }

    public static void main(String[] args) {
	SWIGTYPE_p_Agraph_t g, sg;
	SWIGTYPE_p_Agnode_t n, m;
	SWIGTYPE_p_Agedge_t e;
    byte[] ba;

	g = gv.digraph("G");
	System.out.println(gv.setv(g,"aaa","xxx"));
	System.out.println(gv.getv(g,"aaa"));
	sg = gv.graph(g,"SG");
	n = gv.node(g,"hello");
	System.out.println(gv.getv(n,"label"));
	System.out.println(gv.setv(n,"aaa","xxx"));
	System.out.println(gv.getv(n,"aaa"));
	m = gv.node(g,"world");
        System.out.println(gv.getv(m,"aaa"));
	e = gv.edge(n,m);
	System.out.println(gv.setv(e,"aaa","xxx"));
        System.out.println(gv.getv(e,"aaa"));
	gv.rm(e);
	gv.rm(n);
	gv.rm(m);
	gv.rm(g);

	g = gv.readstring("digraph G {a->b}");
	gv.rm(g);

	g = gv.read("hello.gv");
	gv.layout(g,"dot");
	gv.render(g,"png","hello.png");
	ba = gv.renderresult (g,"dot");
	System.out.println("length = " + ba.length);

	gv.rm(g);
    }
}
