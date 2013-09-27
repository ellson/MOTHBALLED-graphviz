//
// Time-stamp: <30.10.2003 14:33:33h liekweg>
//
// This library is distributed under the BSD license.
// See license.txt for more information.
//
// $Id$
//

package dot;

import java.io.File;
import java.io.FileOutputStream;
import java.io.PrintWriter;

/**
   <P>Represent a Dot(ty) graph.</P>

   @since   Sun Oct 26 16:27:23 2003
   @author  Florian Liekweg <TT>&lt;liekweg@ipd.info.uni-karlsruhe.de&gt;</TT>, Universität Karlsruhe (TH), Germany
 */

public class DotGraph extends DotCluster
{
  /**
     <P>Create a new Graph from the given filename and the given defaults.</P>
  */
  public DotGraph (String graph_title,
                   String filename,
                   DotGraphAttr [] graph_defaults,
                   DotNodeAttr [] node_defaults,
                   DotEdgeAttr [] edge_defaults)
  {
    super (node_defaults);

    _title = graph_title;
    _filename = filename;
    _graph_defaults = graph_defaults;
    _edge_defaults = edge_defaults;

    _clusters = new java.util.Vector ();
    _edges    = new java.util.Vector ();
  }

  /**
     <P>Create a new Graph from the given filename and the given defaults.</P>
  */
  public DotGraph (String graph_title,
                   String filename,
                   DotNodeAttr [] node_defalts,
                   DotEdgeAttr [] edge_defalts)
  {
    this (graph_title, filename, null, node_defalts, edge_defalts);
  }

  /**
     <P>Create a new Graph from the given filename.</P>
  */
  public DotGraph (String graph_title,
                   String filename)
  {
    this (graph_title, filename, null, new DotNodeAttr [0], new DotEdgeAttr [0]);
  }

  // CLUSTER
  /**
     <P>Get a new cluster in this graph.</P>
     @param name        The identifier of the cluster.
     @param label       The text in the cluster.
  */
  public DotCluster get_cluster (String name, String label)
  {
    DotCluster cluster = new DotCluster (_node_defaults, name, label);

    add_cluster (cluster);

    return (cluster);
  }

  /**
     <P>Get a new cluster in this graph.</P>
     @param name        The identifier of the cluster.
  */
  public DotCluster get_cluster (String name)
  {
    DotCluster cluster = new DotCluster (_node_defaults, name);

    add_cluster (cluster);

    return (cluster);
  }

  /**
     <P>Get a new cluster in this graph.</P>
  */
  public DotCluster get_cluster ()
  {
    DotCluster cluster = new DotCluster (_node_defaults);

    add_cluster (cluster);

    return (cluster);
  }

  /**
      <P>Add a cluster to this graph.</P>
  */
  private void add_cluster (DotCluster cluster)
  {
    _clusters.addElement (cluster);
  }

  // EDGES
  /**
     <P>Add an edge from the node havng 'from' as its name to the node that has 'to' as its name.</P>
     @param from    The identifier of the start node.
     @param to      The identifier of the end node.
     @param label   The text in the node.
  */
  public void add_edge (String from, String to)
  {
    _edges.addElement (new DotEdge (from, to, _edge_defaults));
  }

  /**
     <P>Add an edge from the node havng 'from' as its name to the node that has 'to' as its name.</P>
     @param from    The identifier of the start node.
     @param to      The identifier of the end node.
     @param label   The text in the node.
  */
  public void add_edge (String from, String to, DotEdgeAttr [] attrs)
  {
    _edges.addElement (new DotEdge (from, to, attrs));
  }

  // WRITING
  /**
     <P>Write the dot file to disk.</P>
  */
  public void write ()
  {
    PrintWriter out = null;

    try {

      out = new PrintWriter
        (new FileOutputStream
         (new File (_filename)), false);

    } catch (java.io.IOException e) {
      System.out.println (e.toString ());
    }

    write_intro (out);

    write_clusters (out);
    write_nodes (out);
    write_edges (out);

    write_extro (out);
  }

  /**
     <P>Write out all clusters.</P>

     @param out The PrintWriter to write to.
  */
  private void write_clusters (PrintWriter out)
  {
    int n_clusters = _clusters.size ();

    for (int i = 0; i < n_clusters; i ++) {
      DotCluster cluster = (DotCluster) _clusters.elementAt (i);

      cluster.write (out);
    }
  }

  /**
     <P>Write out the graph intro.</P>

     @param out The PrintWriter to write to.
  */
  private void write_intro (PrintWriter out)
  {
    StringBuffer buf;

    out.print ("digraph \"");
    out.print (_title);
    out.print ("\"\n");
    out.print ("{\n");

    buf = new StringBuffer ();
    buf.append ("\t");
    new DotLabelAttr (_title).write (buf);
    buf.append (";\n");

    // graph defaults
    if (null != _graph_defaults) {
      for (int i = 0; i < _graph_defaults.length; i ++) {
        buf.append ("\t");
        _graph_defaults [i].write (buf);

        if (! (_graph_defaults [i] instanceof DotCommentedAttr)) {
          buf.append (";\n");
        } else {
          buf.append ("\n");
        }
      }
    }
    out.print (buf.toString ());

    out.print ("\n");

    // node defaults
    out.print ("\tnode [");

    buf = new StringBuffer ();
    for (int i = 0; i < _node_defaults.length; i ++) {
      _node_defaults [i].write (buf);

      if (i != _node_defaults.length-1)
        buf.append (", ");
    }
    out.print (buf.toString ());
    out.print ("];\n");
    out.print ("\n");

    // edge defaults
    out.print ("\tedge [");

    buf = new StringBuffer ();
    for (int i = 0; i < _edge_defaults.length; i ++) {
      _edge_defaults [i].write (buf);

      if (i != _edge_defaults.length-1)
        buf.append (", ");
    }
    out.print (buf.toString ());
    out.print ("];\n");
    out.print ("\n");
  }

  /**
     <P>Write out all edges.</P>

     @param out The PrintWriter to write to.
  */
  private void write_edges (PrintWriter out)
  {
    StringBuffer buf = new StringBuffer ();

    int n_edges = _edges.size ();

    for (int i = 0; i < n_edges; i ++) {
      DotEdge edge = (DotEdge) _edges.elementAt (i);
      edge.write (buf);
    }

    out.print (buf);
  }

  /**
     <P>Write out the graph extro.</P>

     @param out The PrintWriter to write to.
  */
  private static void write_extro (PrintWriter out)
  {
    out.print ("}\n");
    out.close ();
  }


  private final String _title;
  private final String _filename;
  private final DotEdgeAttr [] _edge_defaults;
  private DotGraphAttr [] _graph_defaults;

  private final java.util.Vector _clusters;
  private final java.util.Vector _edges;
}
