//
// Time-stamp: <30.10.2003 14:33:31h liekweg>
//
// This library is distributed under the BSD license.
// See license.txt for more information.
//
// $Id$
//

package dot;

import java.io.PrintWriter;

/**
   <P>Dot gets Clusters!</P>

   @since   Sun Oct 26 21:56:01 2003
   @author  Florian Liekweg <TT>&lt;liekweg@ipd.info.uni-karlsruhe.de&gt;</TT>, Universität Karlsruhe (TH), Germany
 */

public class DotCluster
{
  /**
     <P>Build a new cluster.</P>
  */
  protected DotCluster (DotNodeAttr [] node_defaults,
                        String name, String label)
  {
    _name = name;
    _label = label;
    _nodes = new java.util.Vector ();
    _node_defaults = node_defaults;
  }

  /**
     <P>Build a new cluster.</P>
  */
  protected DotCluster (DotNodeAttr [] node_defaults,
                        String name)
  {
    this (node_defaults, name, null);
  }

  protected DotCluster (DotNodeAttr [] node_defaults)
  {
    this (node_defaults, "anonymous");
  }

  /**
     <P>Add a node with the given name (identifier for dot), the
     label (text in the node).</P>
     @param name    The identifier (for dot).
     @param label   The text in the node.
  */
  public void add_node (String name, String label)
  {
    _nodes.addElement (new DotNode (name, label, _node_defaults));
  }

  /**
     <P>Add a node with the given name (identifier for dot), the
     label (text in the node) and some more attributes.</P>
     @param name    The identifier (for dot).
     @param label   The text in the node.
     @param attrs   Further attributes.
  */
  public void add_node (String name, String label, DotNodeAttr [] attrs)
  {
    _nodes.addElement (new DotNode (name, label, attrs));
  }

  /**
     <P>Add a node with the given name (identifier for dot) and some more attributes.</P>
     <P>If this node must have a label string, it must be given in the attributes.</P>

     @param name    The identifier (for dot).
     @param attrs   Further attributes.
  */
  public void add_node (String name, DotNodeAttr [] attrs)
  {
    _nodes.addElement (new DotNode (name, attrs));
  }

  public void write (java.io.PrintWriter out)
  {
    StringBuffer buf = new StringBuffer ();

    buf.append ("\tsubgraph \"cluster_" + _name + "\"\n");
    buf.append ("\t{\n");

    if (null != _label) {
      new DotLabelAttr (_label).write (buf);
      buf.append (";\n");
    }

    int n_nodes = _nodes.size ();

    for (int i = 0; i < n_nodes; i ++) {
      DotNode node = (DotNode) _nodes.elementAt (i);
      node.write (buf);
    }

    buf.append ("\t}\n");

    out.print (buf);
  }

  protected void write_nodes (PrintWriter out)
  {
    StringBuffer buf = new StringBuffer ();

    int n_nodes = _nodes.size ();

    for (int i = 0; i < n_nodes; i ++) {
      DotNode node = (DotNode) _nodes.elementAt (i);
      node.write (buf);
    }

    out.print (buf);
  }

  private final String _name;
  private final String _label;
  java.util.Vector _nodes;

  protected final DotNodeAttr [] _node_defaults;
}
