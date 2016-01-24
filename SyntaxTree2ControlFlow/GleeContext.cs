using Microsoft.Glee.Drawing;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Parc
{
  class GleeContext
  {
    private readonly Graph _g;

    public Graph Graph { get { return _g; } }
    public Color Color { get; set; }
    public Color BackLinkEdgeColor { get; set; }
    public NameTable NameTable { get; set; }

    public GleeContext()
    {
      var g = new Graph("g") {
        GraphAttr = {
          LayerDirection = LayerDirection.LR,
        },
      };
      _g = g;
      Color = Color.Black;
      BackLinkEdgeColor = Color.Red;
      NameTable = new NameTable();
    }
  }
}
