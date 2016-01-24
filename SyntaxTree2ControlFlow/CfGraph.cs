using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Parc
{
  public sealed class CfGraph
  {
    private readonly HashSet<CfNode> _nodes = new HashSet<CfNode>(new ReferenceEquality());
    public IEnumerable<CfNode> Nodes { get { return _nodes; } }
    private readonly MultiMap<object, CfEdge> _edgesBySource = new MultiMap<object, CfEdge>(new ReferenceEquality());
    private readonly MultiMap<object, CfEdge> _edgesByTarget = new MultiMap<object, CfEdge>(new ReferenceEquality());
    private int _nodeId = 1;

    public CfGraph()
    {
    }

    public CfGraph(CfGraph copy)
    {
      _nodes = new HashSet<CfNode>(copy._nodes, copy._nodes.Comparer);
      _edgesBySource = new MultiMap<object, CfEdge>(copy._edgesBySource);
      _edgesByTarget = new MultiMap<object, CfEdge>(copy._edgesByTarget);
    }

    public int NextNodeId()
    {
      return _nodeId++;
    }

    public void AddNode(CfNode v)
    {
      _nodes.Add(v);
    }

    public void AddEdge(CfEdge e)
    {
      if (e == null)
      {
        throw new ArgumentNullException(nameof(e));
      }
      if (e.Source == null)
      {
        throw new ArgumentException("Source node cannot be null.");
      }
      if (e.Target == null)
      {
        throw new ArgumentException("Target node cannot be null.");
      }
      _edgesBySource.Add(e.Source, e);
      _edgesByTarget.Add(e.Target, e);
    }

    public void RemoveEdge(CfEdge e)
    {
      RemoveEdge(_edgesBySource, e.Source, e);
      RemoveEdge(_edgesByTarget, e.Target, e);
    }

    private static void RemoveEdge(MultiMap<object, CfEdge> map, CfNode v, CfEdge e)
    {
      List<CfEdge> edges;
      if (map.TryGetValue(v, out edges))
      {
        edges.Remove(e);
      }
    }

    public IEnumerable<CfEdge> GetEdgesBySource(CfNode v)
    {
      List<CfEdge> edges;
      if (_edgesBySource.TryGetValue(v, out edges))
      {
        return edges;
      }
      return Enumerable.Empty<CfEdge>();
    }

    public IEnumerable<CfEdge> GetEdgesByTarget(CfNode v)
    {
      List<CfEdge> edges;
      if (_edgesByTarget.TryGetValue(v, out edges))
      {
        return edges;
      }
      return Enumerable.Empty<CfEdge>();
    }
  }
}
