//
// Time-stamp: <30.10.2003 14:33:38h liekweg>
//
// This library is distributed under the BSD license.
// See license.txt for more information.
//
// $Id$
//

package example;

import dot.*;

/**
   <P>Use the dot package to create a simple graph.</P>

   @version     $Id$
   @since   Thu Oct 30 09:53:40 2003
   @author  Florian Liekweg <TT>&lt;liekweg@ipd.info.uni-karlsruhe.de&gt;</TT>, Universität Karlsruhe (TH), Germany
 */

public class SimpleGraph
  implements DotAttributes // 'import' the attribute names
{
  public static void main (String [] args)
  {
    DotGraph dot = new DotGraph ("Sample Graph", // Graph Title
                                 "sample.gv", // Output file name
                                 GRAPH_DEFAULT, // Graph (default) attributes
                                 NODE_DEFAULT, // node defaults
                                 EDGE_DEFAULT); // edge defaults

    // add some nodes with no special attributes
    dot.add_node ("start",      // node name
                  "Start Node"); // node label

    dot.add_node ("one",
                  "First Node");

    // start a new cluster:
    DotCluster cluster = dot.get_cluster ();
    cluster.add_node ("two",
                      "Second Node\\nFirst Node in Cluster");
    cluster.add_node ("three",
                      "Third Node\\nSecond Node in Cluster");


    dot.add_node ("four",
                  "Fourth Node", RED_NODE);
    dot.add_node ("five",
                  "Fifth Node");

    // now, add some edges:
    dot.add_edge ("start", "one");
    dot.add_edge ("one", "two");
    dot.add_edge ("two", "three");
    dot.add_edge ("three", "four");
    dot.add_edge ("four", "five");

    // write it to disk:
    dot.write ();
  }

  private static DotGraphAttr [] GRAPH_DEFAULT = {GRAPH_TD,
                                                  new DotCommentedAttr
                                                  (GRAPH_A4_LANDSCAPE)};

  private static DotNodeAttr [] NODE_DEFAULT = {NODE_NORMAL, NODE_RECORD,
                                                new DotStringAttr (FONTSIZE, "12"),
                                                new DotStringAttr (STYLE,
                                                                   STYLE_FILLED),
                                                new DotColorAttr ("lightblue")};

  private static DotNodeAttr [] RED_NODE = {NODE_RED};

  private static DotEdgeAttr [] EDGE_DEFAULT = {EDGE_NORMAL,
                                                new DotStringAttr (FONTSIZE, "10"),
                                                EDGE_BLACK};
}


/*
  Local Variables:
  c-basic-offset: 2
  End:
*/
