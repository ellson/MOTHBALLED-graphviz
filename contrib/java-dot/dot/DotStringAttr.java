//
// Time-stamp: <30.10.2003 14:33:37h liekweg>
//
// This library is distributed under the BSD license.
// See license.txt for more information.
//
// $Id$
//

package dot;

/**
   <P>Quick and ugly hack to MAKE DOT NODE ATTRIBUTES FAST.</P>

   @since   Sun Oct 26 16:53:16 2003
   @author  Florian Liekweg <TT>&lt;liekweg@ipd.info.uni-karlsruhe.de&gt;</TT>, Universität Karlsruhe (TH), Germany
 */

public class DotStringAttr implements DotNodeAttr, DotEdgeAttr, DotGraphAttr
{
  public DotStringAttr (String key, String value)
  {
    _key = key;
    _value = value;
  }

  public void write (StringBuffer buf)
  {
    buf.append (_key);
    buf.append ("=\"");
    buf.append (_value);
    buf.append ("\"");
  }

  private final String _key;
  private final String _value;
}
