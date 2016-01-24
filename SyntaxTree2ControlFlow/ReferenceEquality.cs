using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;

namespace Parc
{
  public class ReferenceEquality : IEqualityComparer<object>
  {
    public new bool Equals(object x, object y)
    {
      return object.ReferenceEquals(x, y);
    }

    public int GetHashCode(object obj)
    {
      return RuntimeHelpers.GetHashCode(obj);
    }
  }
}
