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
   <P>Dot Attribute values</P>

   @since   Sun Oct 26 16:51:49 2003
   @author  Florian Liekweg <TT>&lt;liekweg@ipd.info.uni-karlsruhe.de&gt;</TT>, Universität Karlsruhe (TH), Germany
 */

public interface DotAttrValues
{
  // GRAPH
  String TOP_DOWN = "TD";
  String LEFT_RIGHT = "LR";

  String FILL = "fill";

  // STYLE
  String STYLE_NORMAL = "normal";
  String STYLE_BOLD   = "bold";
  String STYLE_DOTTED = "dotted";
  String STYLE_FILLED = "filled";

  // SHAPE
  String SHAPE_RECORD   = "record";
  String SHAPE_ELLIPSE  = "ellipse";
  String SHAPE_CIRCLE   = "circle";
  String SHAPE_TRIANGLE = "triangle";

  // COLOR (todo)

  // DIR
  String DIR_BACK = "back";
  String DIR_NONE = "none";
  String DIR_BOTH = "both";
}

