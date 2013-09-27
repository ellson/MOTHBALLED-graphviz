//
// Time-stamp: <30.10.2003 14:33:30h liekweg>
//
// This library is distributed under the BSD license.
// See license.txt for more information.
//
// $Id$
//

package dot;

/**
   <P>Common Dot Attributes</P>

   @since   Sun Oct 26 16:52:13 2003
   @author  Florian Liekweg <TT>&lt;liekweg@ipd.info.uni-karlsruhe.de&gt;</TT>, Universität Karlsruhe (TH), Germany
 */

public interface DotAttributes extends DotAttrNames, DotAttrValues
{
  // graph
  DotGraphAttr GRAPH_TD     = new DotStringAttr (RANKDIR, TOP_DOWN);
  DotGraphAttr GRAPH_LR     = new DotStringAttr (RANKDIR, LEFT_RIGHT);
  DotGraphAttr GRAPH_FILL   = new DotStringAttr (RATIO, FILL);
  DotGraphAttr GRAPH_A4_LANDSCAPE = new DotGraphSizeAttr (11, 7);
  DotGraphAttr GRAPH_A4_PORTRAIT  = new DotGraphSizeAttr (7, 11);
  DotGraphAttr GRAPH_ROTATE_90  = new DotGraphRotateAttr (90);
  DotGraphAttr GRAPH_ROTATE_180 = new DotGraphRotateAttr (180);

  // nodes
  DotNodeAttr NODE_BLUE     = new DotStringAttr (COLOR, "blue");
  DotNodeAttr NODE_BLUE1    = new DotStringAttr (COLOR, "blue1");
  DotNodeAttr NODE_BLUE2    = new DotStringAttr (COLOR, "blue2");
  DotNodeAttr NODE_BLUE3    = new DotStringAttr (COLOR, "blue3");
  DotNodeAttr NODE_BLUE4    = new DotStringAttr (COLOR, "blue4");
  DotNodeAttr NODE_GREEN    = new DotStringAttr (COLOR, "green");
  DotNodeAttr NODE_GREEN1   = new DotStringAttr (COLOR, "green1");
  DotNodeAttr NODE_GREEN2   = new DotStringAttr (COLOR, "green2");
  DotNodeAttr NODE_GREEN3   = new DotStringAttr (COLOR, "green3");
  DotNodeAttr NODE_GREEN4   = new DotStringAttr (COLOR, "green4");
  DotNodeAttr NODE_RED      = new DotStringAttr (COLOR, "red");
  DotNodeAttr NODE_RED1     = new DotStringAttr (COLOR, "red1");
  DotNodeAttr NODE_RED2     = new DotStringAttr (COLOR, "red2");
  DotNodeAttr NODE_RED3     = new DotStringAttr (COLOR, "red3");
  DotNodeAttr NODE_RED4     = new DotStringAttr (COLOR, "red4");
  DotNodeAttr NODE_YELLOW   = new DotStringAttr (COLOR, "yellow");
  DotNodeAttr NODE_YELLOW1  = new DotStringAttr (COLOR, "yellow1");
  DotNodeAttr NODE_YELLOW2  = new DotStringAttr (COLOR, "yellow2");
  DotNodeAttr NODE_YELLOW3  = new DotStringAttr (COLOR, "yellow3");
  DotNodeAttr NODE_YELLOW4  = new DotStringAttr (COLOR, "yellow4");
  DotNodeAttr NODE_WHITE    = new DotStringAttr (COLOR, "white");
  DotNodeAttr NODE_ORANGE   = new DotStringAttr (COLOR, "orange");
  DotNodeAttr NODE_BLACK    = new DotStringAttr (COLOR, "black");
  DotNodeAttr NODE_GREY     = new DotStringAttr (COLOR, "grey");

  DotNodeAttr NODE_LBLUE    = new DotStringAttr (COLOR, "lightblue");
  DotNodeAttr NODE_LYELLOW  = new DotStringAttr (COLOR, "lightyellow");
  DotNodeAttr NODE_LGREY    = new DotStringAttr (COLOR, "lightgrey");

  DotNodeAttr NODE_BOLD     = new DotStringAttr (FONTNAME, "Helvetica-Bold");
  DotNodeAttr NODE_NORMAL   = new DotStringAttr (FONTNAME, "Helvetica");

  DotNodeAttr NODE_RECORD   = new DotStringAttr (SHAPE, SHAPE_RECORD);
  DotNodeAttr NODE_TRIANGLE = new DotStringAttr (SHAPE, SHAPE_TRIANGLE);
  DotNodeAttr NODE_CIRCLE   = new DotStringAttr (SHAPE, SHAPE_CIRCLE);
  DotNodeAttr NODE_ELLIPSE  = new DotStringAttr (SHAPE, SHAPE_ELLIPSE);

  DotNodeAttr NODE_FILLED   = new DotStringAttr (STYLE, STYLE_FILLED);

  // edges
  DotEdgeAttr EDGE_NORMAL   = new DotStringAttr (STYLE, STYLE_NORMAL);
  DotEdgeAttr EDGE_BOLD     = new DotStringAttr (STYLE, STYLE_BOLD);
  DotEdgeAttr EDGE_DOTTED   = new DotStringAttr (STYLE, STYLE_DOTTED);

  DotEdgeAttr EDGE_BLUE     = new DotStringAttr (COLOR, "blue");
  DotEdgeAttr EDGE_BLUE1    = new DotStringAttr (COLOR, "blue1");
  DotEdgeAttr EDGE_BLUE2    = new DotStringAttr (COLOR, "blue2");
  DotEdgeAttr EDGE_BLUE3    = new DotStringAttr (COLOR, "blue3");
  DotEdgeAttr EDGE_BLUE4    = new DotStringAttr (COLOR, "blue4");
  DotEdgeAttr EDGE_GREEN1   = new DotStringAttr (COLOR, "green");
  DotEdgeAttr EDGE_GREEN2   = new DotStringAttr (COLOR, "green1");
  DotEdgeAttr EDGE_GREEN3   = new DotStringAttr (COLOR, "green2");
  DotEdgeAttr EDGE_GREEN4   = new DotStringAttr (COLOR, "green3");
  DotEdgeAttr EDGE_GREEN    = new DotStringAttr (COLOR, "green4");
  DotEdgeAttr EDGE_RED      = new DotStringAttr (COLOR, "red");
  DotEdgeAttr EDGE_RED1     = new DotStringAttr (COLOR, "red1");
  DotEdgeAttr EDGE_RED2     = new DotStringAttr (COLOR, "red2");
  DotEdgeAttr EDGE_RED3     = new DotStringAttr (COLOR, "red3");
  DotEdgeAttr EDGE_RED4     = new DotStringAttr (COLOR, "red4");
  DotEdgeAttr EDGE_YELLOW   = new DotStringAttr (COLOR, "yellow");
  DotEdgeAttr EDGE_YELLOW1  = new DotStringAttr (COLOR, "yellow1");
  DotEdgeAttr EDGE_YELLOW2  = new DotStringAttr (COLOR, "yellow2");
  DotEdgeAttr EDGE_YELLOW3  = new DotStringAttr (COLOR, "yellow3");
  DotEdgeAttr EDGE_YELLOW4  = new DotStringAttr (COLOR, "yellow4");
  DotEdgeAttr EDGE_WHITE    = new DotStringAttr (COLOR, "white");
  DotEdgeAttr EDGE_ORANGE   = new DotStringAttr (COLOR, "orange");
  DotEdgeAttr EDGE_BLACK    = new DotStringAttr (COLOR, "black");
  DotEdgeAttr EDGE_GREY     = new DotStringAttr (COLOR, "grey");
  DotEdgeAttr EDGE_LGREY    = new DotStringAttr (COLOR, "lightgrey");

  DotEdgeAttr EDGE_BACK     = new DotStringAttr (DIR, DIR_BACK);
  DotEdgeAttr EDGE_NONE     = new DotStringAttr (DIR, DIR_NONE);
  DotEdgeAttr EDGE_BOTH     = new DotStringAttr (DIR, DIR_BOTH);
}
