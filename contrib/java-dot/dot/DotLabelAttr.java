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
   <P>Quick and dirty hack to MAKE LABELS FAST.</P>

   @since   Sun Oct 26 22:08:55 2003
   @author  Florian Liekweg <TT>&lt;liekweg@ipd.info.uni-karlsruhe.de&gt;</TT>, Universität Karlsruhe (TH), Germany
 */

public class DotLabelAttr extends DotStringAttr
{
  /**
     <P></P>
  */
  public DotLabelAttr (String label)
  {
    super ("label", label);
  }
}
