using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Parc
{
  class NameTable
  {
    private readonly char[] _subscriptBase = new[] { '₀', '₁', '₂', '₃', '₄', '₅', '₆', '₇', '₈', '₉' };
    private readonly Dictionary<object, string> _nameTable = new Dictionary<object, string>(new ReferenceEquality());

    public string ToSubscript(int n)
    {
      var sb = new StringBuilder();
      do
      {
        sb.Insert(0, _subscriptBase[n % 10]);
        n /= 10;
      }
      while (n != 0);
      var s = sb.ToString();
      return s;
    }

    public void Add(object key, string label)
    {
      _nameTable.Add(key, label);
    }

    public bool TryAdd(object key, string s, int? n, out string label)
    {
      if (!_nameTable.TryGetValue(key, out label))
      {
        _nameTable.Add(key, label = s + ToSubscript(n ?? _nameTable.Count));
        return true;
      }
      return false;
    }
  }
}
