using Microsoft.Glee.Drawing;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Parc
{
  static class GraphVisual
  {
    public static Graph CreateVisual(Compilation compilation)
    {
      var g = new Graph("DFA") {
        GraphAttr = {
          LayerDirection = LayerDirection.LR,
        },
      };
      var sc = new DfaConstruction();
      var nt = new NameTable();
      foreach (var s in compilation.Graph.Nodes)
      {
        var fsm = sc.Construct(s, compilation.Graph);
        nt.Add(fsm, s.Label);
        CreateVisual(g, fsm, nt);
        var root = g.FindNode(s.Label);
        root.Attr.Color = Color.YellowGreen;
      }
      return g;
    }

    private static void CreateVisual(Graph g, DfaState fsm, NameTable nt)
    {
      foreach (var state in fsm.Traverse())
      {
        string nodeId = CreateNode(g, state, nt);
        if (state.HasTransitions)
        {
          foreach (var t in state.Transitions)
          {
            var targetNodeId = CreateNode(g, t.Target, nt);
            var transitionEdge = g.AddEdge(nodeId, t.Input, targetNodeId);
            ApplyDefaultStyle(transitionEdge);
          }
        }
      }
    }

    private static string CreateNode(Graph g, DfaState state, NameTable nt)
    {
      string label;
      nt.TryAdd(state, "s", null, out label);
      var stateNode = g.FindNode(label);
      if (stateNode == null)
      {
        stateNode = g.AddNode(label);
        ApplyDefaultStyle(stateNode);
        if (state.IsAcceptState)
        {
          stateNode.Attr.Shape = Shape.DoubleCircle;
        }
      }
      return label;
    }

    private static void ApplyDefaultStyle(Node stateNode)
    {
      stateNode.Attr.Shape = Shape.Circle;
      stateNode.Attr.FontName = "Consolas";
      stateNode.Attr.LabelMargin = 3;
    }

    private static void ApplyDefaultStyle(Edge transitionEdge)
    {
      transitionEdge.EdgeAttr.FontName = "Consolas";
    }
  }
}
