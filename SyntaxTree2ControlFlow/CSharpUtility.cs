using System;
using System.CodeDom;
using System.CodeDom.Compiler;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Parc
{
  static class CSharpUtility
  {
    public static string ToLiteral(string s)
    {
      using (var writer = new StringWriter())
      {
        using (var provider = CodeDomProvider.CreateProvider("CSharp"))
        {
          provider.GenerateCodeFromExpression(new CodePrimitiveExpression(s), writer, null);
        }
        return writer.ToString();
      }
    }
  }
}
