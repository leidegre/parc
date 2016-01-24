using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Microsoft.Glee.Drawing;

namespace Parc
{
  class ControlFlowNode
  {
    public bool HasEdges { get { return _edges != null && _edges.Count > 0; } }
    private List<ControlFlowEdge> _edges;
    public IEnumerable<ControlFlowEdge> Edges { get { return _edges ?? Enumerable.Empty<ControlFlowEdge>(); } }

    public bool IsAcceptingState { get; internal set; }
    public string Projection { get; internal set; }

    public void AddEdge(ControlFlowEdge edge)
    {
      if (edge.Source != null)
      {
        throw new ArgumentException("This edge belongs to a different node.");
      }
      if (edge.Target == null)
      {
        throw new ArgumentException("This edge does not point to a node.");
      }
      if (_edges == null)
      {
        _edges = new List<ControlFlowEdge>();
      }
      edge.Source = this;
      _edges.Add(edge);
    }
  }

  abstract class ControlFlowEdge
  {
    public ControlFlowNode Source { get; set; }
    public ControlFlowNode Target { get; set; }
    internal abstract void SetVisualStyle(Edge visualEdge);
  }

  class ControlFlowTokenReferenceEdge : ControlFlowEdge
  {
    public SyntaxNode SyntaxNode { get; }
    public string Identifier { get; }

    public ControlFlowTokenReferenceEdge(SyntaxNode SyntaxNode, string identifier)
    {
      this.SyntaxNode = SyntaxNode;
      this.Identifier = identifier;
    }

    internal override void SetVisualStyle(Edge visualEdge)
    {
      visualEdge.Attr.Label = "[" + Identifier + "]";
    }
  }

  class ControlFlowProductionReferenceEdge : ControlFlowEdge
  {
    public SyntaxNode SyntaxNode { get; }
    public string Identifier { get; }

    public ControlFlowProductionReferenceEdge(SyntaxNode SyntaxNode, string identifier)
    {
      this.SyntaxNode = SyntaxNode;
      this.Identifier = identifier;
    }

    internal override void SetVisualStyle(Edge visualEdge)
    {
      visualEdge.Attr.Label = "[" + Identifier + "]";
    }
  }

  class ControlFlowJumpEdge : ControlFlowEdge
  {
    internal override void SetVisualStyle(Edge visualEdge)
    {
    }
  }
}
