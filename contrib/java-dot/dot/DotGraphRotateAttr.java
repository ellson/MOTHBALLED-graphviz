//
// Time-stamp: <30.10.2003 14:33:34h liekweg>
//
// This library is distributed under the BSD license.
// See license.txt for more information.
//
// $Id$
//

package dot;

/**
   <P>Quick and ugly hack to MAKE DOT GRAPH ROTATION FAST.</P>

   @since   Mon Oct 27 20:52:23 2003
   @author  Florian Liekweg <TT>&lt;liekweg@ipd.info.uni-karlsruhe.de&gt;</TT>, Universität Karlsruhe (TH), Germany
 */

public class DotGraphRotateAttr implements DotGraphAttr
{
  public DotGraphRotateAttr (int angle)
  {
    _angle = new DotStringAttr (DotAttrNames.ROTATE, angle + "");
  }

  public void write (StringBuffer buf)
  {
    _angle.write (buf);
  }

  private final DotStringAttr _angle;
}
