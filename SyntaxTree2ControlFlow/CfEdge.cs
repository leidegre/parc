using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Parc
{
  public class CfEdge
  {
    public CfNode Source { get; set; }
    public CfNode Target { get; set; }
    private HashSet<string> _guardSet;
    public bool HasGuardSet { get { return _guardSet != null && _guardSet.Count > 0; } }
    public HashSet<string> GuardSet { get { return _guardSet ?? (_guardSet = new HashSet<string>()); } set { _guardSet = value; } }

    /// <summary>
    /// Gets or sets whether this edge results in a cycle or not.
    /// </summary>
    public bool IsBackLink { get; internal set; }
  }
}
