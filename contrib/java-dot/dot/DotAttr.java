//
// Time-stamp: <30.10.2003 14:33:25h liekweg>
//
// This library is distributed under the BSD license.
// See license.txt for more information.
//
// $Id$
//

package dot;

/**
   <P>Super Interface for all Attributes</P>

   @since   Mon Oct 27 20:32:40 2003
   @author  Florian Liekweg <TT>&lt;liekweg@ipd.info.uni-karlsruhe.de&gt;</TT>, Universität Karlsruhe (TH), Germany
 */

public interface DotAttr extends DotAttrNames, DotAttrValues
{
  public abstract void write (StringBuffer buf);
}
