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
   <P></P>

   @since   Sun Oct 26 21:59:35 2003
   @author  Florian Liekweg <TT>&lt;liekweg@ipd.info.uni-karlsruhe.de&gt;</TT>, Universität Karlsruhe (TH), Germany
 */

public class DotEdge
{
  public DotEdge (String from, String to, DotEdgeAttr [] attrs)
  {
    _from = from;
    _to = to;
    _attrs = attrs;

    if (null == _attrs)
      throw (new Error ("null attrs"));
  }

  public void write (StringBuffer buf)
  {
    buf.append ("\t");
    buf.append (_from);
    buf.append (" -> ");
    buf.append (_to);

    buf.append (" [\n\t\t");

    for (int i = 0;  i < _attrs.length; i ++) {
      _attrs [i].write (buf);
      if (i != _attrs.length-1) {
        buf.append (",\n\t\t");
      }
    }

    buf.append ("];\n");
  }

  private final String _from;
  private final String _to;
  private final DotEdgeAttr [] _attrs;
}
