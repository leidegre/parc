using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Parc
{
  public class MultiMap<TKey, TValue> : Dictionary<TKey, List<TValue>>
  {
    public MultiMap(IEqualityComparer<TKey> comparer)
      : base(comparer)
    {
    }

    public MultiMap(MultiMap<TKey, TValue> copy)
      : base(copy.Comparer)
    {
      foreach (var item in copy)
      {
        Add(item.Key, new List<TValue>(item.Value));
      }
    }

    public void Add(TKey key, TValue value)
    {
      List<TValue> list;
      if (TryGetValue(key, out list))
      {
        list.Add(value);
      }
      else
      {
        Add(key, list = new List<TValue> { value });
      }
    }
  }
}
