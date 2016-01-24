using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Parc
{
  public class Compilation
  {
    public CfGraph Graph { get; set; }

    public Compilation()
    {
      Graph = new CfGraph();
    }
  }
}
