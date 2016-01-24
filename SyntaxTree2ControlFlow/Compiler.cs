using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Parc
{
  class Compiler
  {
    public Compilation Compile(SyntaxTree syntax)
    {
      var unit = new Compilation();

      var binder = new Binder(unit);

      syntax.Accept(binder);

      return unit;
    }
  }
}
