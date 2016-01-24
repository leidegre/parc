using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Parc
{
  class ListStack<T> : IEnumerable<T>
  {
    private readonly List<T> _list = new List<T>();

    public int Count { get { return _list.Count; } }

    public T this[int index] { get { return _list[index]; } }

    public T Peek(int backtrack = 0)
    {
      if (_list.Count == 0)
      {
        throw new InvalidOperationException("The stack is empty.");
      }
      return _list[_list.Count - (backtrack + 1)];
    }

    public void Push(T value)
    {
      _list.Add(value);
    }

    public void Pop()
    {
      if (_list.Count == 0)
      {
        throw new InvalidOperationException("The stack is empty.");
      }
      _list.RemoveAt(_list.Count - 1);
    }

    /// <summary>
    /// Duplicate the top most value value of the stack [tmp = peek, push(tmp)].
    /// </summary>
    public void Duplicate()
    {
      var top = Peek();
      Push(top);
    }

    /// <summary>
    /// Replace the top most value value of the stack [pop, push].
    /// </summary>
    public void Replace(T newValue)
    {
      Pop();
      Push(newValue);
    }

    /// <summary>
    /// Remove the second top most value of the stack [tmp = peek, pop, pop, push(tmp)].
    /// </summary>
    public void Bubble()
    {
      var oldValue = Peek();
      Pop();
      Replace(oldValue);
    }

    public IEnumerator<T> GetEnumerator()
    {
      return _list.GetEnumerator();
    }

    IEnumerator IEnumerable.GetEnumerator()
    {
      return GetEnumerator();
    }
  }
}
