//
// Time-stamp: <30.10.2003 14:33:29h liekweg>
//
// This library is distributed under the BSD license.
// See license.txt for more information.
//
// $Id$
//

package dot;

/**
   <P>Dot Attribute Names</P>

   @since   Sun Oct 26 16:51:22 2003
   @author  Florian Liekweg <TT>&lt;liekweg@ipd.info.uni-karlsruhe.de&gt;</TT>, Universität Karlsruhe (TH), Germany
 */

public interface DotAttrNames
{
  // graph:
  String RANKDIR = "rankdir";
  String RATIO   = "ratio";
  String SIZE    = "size";
  String ROTATE  = "rotate";

  // mixed and node:
  String COLOR = "color";
  String FONTCOLOR = "fontcolor";
  String FONTNAME = "fontname";
  String FONTSIZE = "fontsize";
  String LABEL = "label";
  String SHAPE = "shape";

  String STYLE = "style";

  // edge:
  String DIR = "dir";
  String WEIGHT = "weight";
}

