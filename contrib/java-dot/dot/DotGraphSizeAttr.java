//
// Time-stamp: <30.10.2003 14:33:35h liekweg>
//
// This library is distributed under the BSD license.
// See license.txt for more information.
//
// $Id$
//

package dot;

/**
   <P>Quick and ugly hack to MAKE DOT GRAPH SIZE FAST.</P>

   @since   Mon Oct 27 20:52:23 2003
   @author  Florian Liekweg <TT>&lt;liekweg@ipd.info.uni-karlsruhe.de&gt;</TT>, Universität Karlsruhe (TH), Germany
 */

public class DotGraphSizeAttr implements DotGraphAttr
{
  public DotGraphSizeAttr (int x, int y)
  {
    _size = new DotStringAttr (DotAttrNames.SIZE, x + ", " + y);
  }

  public void write (StringBuffer buf)
  {
    _size.write (buf);
  }

  private final DotStringAttr _size;
}
