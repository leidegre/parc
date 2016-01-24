using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Parc
{
  public class DfaState : IEquatable<DfaState>
  {
    private readonly List<CfNode> _state = new List<CfNode>();
    public IEnumerable<CfNode> State { get { return _state; } }

    private List<DfaTransition> _transitions;
    public bool HasTransitions { get { return _transitions != null && _transitions.Count > 0; } }
    public IEnumerable<DfaTransition> Transitions { get { return _transitions ?? Enumerable.Empty<DfaTransition>(); } }

    private int[] _id;
    public int[] Id
    {
      get
      {
        if (_id == null)
        {
          _id = _state.Select(x => x.Id).OrderBy(x => x).ToArray();
        }
        return _id;
      }
    }

    private bool? _isAcceptState;
    public bool IsAcceptState
    {
      get
      {
        if (_isAcceptState == null)
        {
          _isAcceptState = _state.Any(x => x.Projection != null);
        }
        return _isAcceptState.Value;
      }
    }

    public bool Add(CfNode cfNode)
    {
      if (_state.Contains(cfNode))
      {
        return false;
      }
      _state.Add(cfNode);
      _id = null; // dirty
      return true;
    }

    public void AddTransition(string input, DfaState target)
    {
      if (_transitions == null)
      {
        _transitions = new List<DfaTransition>();
      }
      _transitions.Add(new DfaTransition {
        Input = input,
        Target = target,
      });
    }

    public IEnumerable<DfaState> Traverse()
    {
      var visit = new HashSet<DfaState>();
      var stack = new Stack<DfaState>();
      stack.Push(this);
      while (stack.Count > 0)
      {
        var currState = stack.Peek();
        stack.Pop();
        if (currState.HasTransitions)
        {
          foreach (var nextState in currState.Transitions.Reverse())
          {
            if (visit.Add(nextState.Target))
            {
              stack.Push(nextState.Target);
            }
          }
        }
        yield return currState;
      }
    }

    public bool Equals(DfaState other)
    {
      return this.Id.SequenceEqual(other.Id);
    }

    public override bool Equals(object obj)
    {
      var value = obj as DfaState;
      if (value != null)
      {
        return Equals(value);
      }
      return false;
    }

    static uint ComputeHash(uint a)
    {
      a = (a ^ 61) ^ (a >> 16);
      a = a + (a << 3);
      a = a ^ (a >> 4);
      a = a * 0x27d4eb2dU;
      a = a ^ (a >> 15);
      return a;
    }

    static int ComputeHash(int a)
    {
      return (int)ComputeHash((uint)a);
    }

    public override int GetHashCode()
    {
      int hashCode = 0;
      foreach (var s in _state)
      {
        hashCode ^= ComputeHash(s.Id);
      }
      return hashCode;
    }

    public override string ToString()
    {
      return "{" + string.Join(", ", Id) + "}";
    }
  }

  public class DfaTransition
  {
    public string Input { get; set; }
    public DfaState Target { get; set; }

    public override string ToString()
    {
      return Input;
    }
  }

  public class DfaConstruction
  {
    public DfaState Construct(CfNode root, CfGraph g)
    {
      // this is where our guard sets are useful, they now allow us to use this
      // basic algorithm to build the DFA from symbols alone which is simple
      var initialState = new DfaState();
      Closure(g, root, initialState);
      Construct(g, initialState, new Dictionary<DfaState, DfaState>());
      return initialState;
    }

    private static void Construct(CfGraph g, DfaState currState, Dictionary<DfaState, DfaState> visit)
    {
      var input = currState.State
        .SelectMany(x => g.GetEdgesBySource(x))
        .Where(x => x.HasGuardSet)
        .SelectMany(x => x.GuardSet);

      foreach (var x in input)
      {
        var newState = new DfaState();
        Move(g, currState, x, newState);
        DfaState prevState;
        if (visit.TryGetValue(newState, out prevState))
        {
          currState.AddTransition(x, prevState);
        }
        else
        {
          visit.Add(newState, newState);
          currState.AddTransition(x, newState);
          Construct(g, newState, visit);
        }
      }
    }

    /// <summary>
    /// Compute the so called ɛ-closure.
    /// </summary>
    private static void Closure(CfGraph g, CfNode nfaState, DfaState dfaState)
    {
      if (dfaState.Add(nfaState)) // a set implies uniqueness, also it prevents "loops"
      {
        foreach (var e in g.GetEdgesBySource(nfaState).Where(x => !x.HasGuardSet))
        {
          Closure(g, e.Target, dfaState);
        }
      }
    }

    private static void Move(CfGraph g, DfaState state, string x, DfaState newState)
    {
      foreach (var s in state.State)
      {
        foreach (var e in g.GetEdgesBySource(s))
        {
          if (e.HasGuardSet && e.GuardSet.Contains(x))
          {
            Closure(g, e.Target, newState);
          }
        }
      }
    }
  }
}
