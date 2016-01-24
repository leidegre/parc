using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Parc
{
  public class CfNode
  {
    public int Id { get; private set; }
    public string Label { get; set; }
    public CfProjection Projection { get; set; }

    public CfNode(CfGraph g)
    {
      Id = g.NextNodeId();
    }

    public override string ToString()
    {
      return Id.ToString();
    }
  }
}
