//
// Time-stamp: <30.10.2003 14:33:32h liekweg>
//
// This library is distributed under the BSD license.
// See license.txt for more information.
//
// $Id$
//

package dot;

/**
   <P>Quick and ugly hack to MAKE COMMENTS FAST</P>

   @since   Mon Oct 27 20:56:28 2003
   @author  Florian Liekweg <TT>&lt;liekweg@ipd.info.uni-karlsruhe.de&gt;</TT>, Universität Karlsruhe (TH), Germany
 */

public class DotCommentedAttr implements DotNodeAttr, DotEdgeAttr, DotGraphAttr
{
  public DotCommentedAttr (DotAttr attr)
  {
    _attr = attr;
  }

  public void write (StringBuffer buf)
  {
    buf.append ("/* ");
    _attr.write (buf);
    buf.append (" */");
  }

  private final DotAttr _attr;
}
