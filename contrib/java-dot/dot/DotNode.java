//
// Time-stamp: <30.10.2003 14:33:36h liekweg>
//
// This library is distributed under the BSD license.
// See license.txt for more information.
//
// $Id$
//

package dot;

/**
   <P></P>

   @since   Sun Oct 26 21:59:20 2003
   @author  Florian Liekweg <TT>&lt;liekweg@ipd.info.uni-karlsruhe.de&gt;</TT>, Universität Karlsruhe (TH), Germany
 */

public class DotNode
{
  /**
     <P>Create a node with the given name (identifier for dot), the
     label (text in the node) and some more attributes.</P>
     @param name    The identifier (for dot)
     @param label   The text in the node
     @param attrs   Further attributes
  */
  public DotNode (String name, String label, DotNodeAttr [] attrs)
  {
    _name = name;
    _label = label;
    _attrs = attrs;
  }

  /**
     <P>Create a node with the given name (identifier for dot) and some more attributes.</P>
     @param name    The identifier (for dot)
     @param attrs   Further attributes
  */
  public DotNode (String name, DotNodeAttr [] attrs)
  {
    this (name, "", attrs);
  }

  public void write (StringBuffer buf)
  {
    buf.append ("\t");
    buf.append (_name);
    buf.append (" [\n\t\t");

    new DotStringAttr (DotAttrNames.LABEL, _label).write (buf);

    if (0 != _attrs.length)
      buf.append (",\n\t\t");

    for (int i = 0;  i < _attrs.length; i ++) {
      _attrs [i].write (buf);
      if (i != _attrs.length-1) {
        buf.append (",\n\t\t");
      }
    }

    buf.append ("];\n");
  }

  private final String _name;
  private final String _label;
  private final DotNodeAttr [] _attrs;
}
