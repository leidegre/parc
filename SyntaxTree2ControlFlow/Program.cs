using Microsoft.Glee.Drawing;
using Parc.UI;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;

namespace Parc
{
  class Program
  {
    // 1: local_identifier
    // 2: global_identifier
    // 3: bind
    // 4: alteration
    // 5: end
    // 6: literal
    // 7: l_brace
    // 8: r_brace

    // PrimaryExpression
    //  = number
    //  , l_paren Expression r_paren
    //  ;
    // Expression
    //  = PrimaryExpression
    //  , PrimaryExpression operator Expression
    //  ;

    [STAThread]
    static void Main(string[] args)
    {
      var container = new Container();
      container.RegisterSingleton(() => {
        return new CompilerInfrastructure {
          Text = @"""parc"" {
  A = a (b | c) + (d | e) f ;
}"
        };
      });
      var wm = new WindowManager();
      container.Setup(wm);
      var window = new System.Windows.Window();
      window.Content = wm;
      var app = new System.Windows.Application();
      app.Run(window);
    }

    public static SyntaxTree ParseText(string source)
    {
      var lexer = new Lexer(source);
      var parser = new Parser(lexer);
      parser.ParseGrammarUnit();
      var syntax = parser.SyntaxTree;
      return syntax;
    }

    public static Graph CreateVisualGraph(Compilation compilation)
    {
      var glee = new GleeContext();
      foreach (var v in compilation.Graph.Nodes)
      {
        glee.NameTable.Add(v, v.Label);
        CreateVisualGraph(compilation.Graph, v, glee);
        var root= glee.Graph.FindNode(v.Label);
        root.Attr.Color = Color.YellowGreen;
      }
      return glee.Graph;
    }

    private static void CreateVisualGraph(CfGraph g, CfNode v, GleeContext glee)
    {
      string sourceId;
      glee.NameTable.TryAdd(v, "s", v.Id, out sourceId);
      var newNode = glee.Graph.AddNode(sourceId);
      newNode.Attr.Shape = Shape.Circle;
      newNode.Attr.FontName = "Consolas";
      newNode.Attr.LabelMargin = 3;
      newNode.Attr.Color = glee.Color;
      var edges = g.GetEdgesBySource(v);
      if (edges.Where(x => !x.HasGuardSet).Count() > 1)
      {
        newNode.Attr.Shape = Shape.Diamond;
      }
      if (v.Projection != null)
      {
        newNode.Attr.Shape = Shape.DoubleCircle;
      }
      foreach (var e in edges)
      {
        string targetId;
        bool isNew = glee.NameTable.TryAdd(e.Target, "s", e.Target.Id, out targetId);
        var newEdge = glee.Graph.AddEdge(sourceId, targetId);
        newEdge.EdgeAttr.Label = e.HasGuardSet ? "[" + string.Join(",", e.GuardSet) + "]" : string.Empty;
        newEdge.EdgeAttr.FontName = "Consolas";
        newEdge.EdgeAttr.Color = glee.Color;
        if (e.IsBackLink)
        {
          newEdge.EdgeAttr.Color = glee.BackLinkEdgeColor;
        }
        if (!e.HasGuardSet) // ɛ-move
        {
          var c = newEdge.EdgeAttr.Color;
          newEdge.EdgeAttr.Color = new Microsoft.Glee.Drawing.Color((byte)(c.A / 2), c.R, c.G, c.B);
        }
        if (isNew) // loop detection
        {
          CreateVisualGraph(g, e.Target, glee);
        }
      }
    }

    public static void Optimize(Compilation compilation)
    {
      foreach (var v in compilation.Graph.Nodes)
      {
        var visited = new HashSet<object>(new ReferenceEquality());

        int i = 0;
        while (OptimizationPass(compilation.Graph, v, visited) > 0)
        {
          visited.Clear();
          i++;
        }

        Console.WriteLine("#{0} optimization pass(es)", i);
      }
    }

    private static int OptimizationPass(CfGraph g, CfNode v, HashSet<object> visited)
    {
      var m = 0;
      if (visited.Add(v))
      {
        var edges = g.GetEdgesBySource(v);
        if (edges.Count() == 1 && !edges.First().HasGuardSet)
        {
          // ɛ-move
          var next = edges.First().Target;
          var edges2 = g.GetEdgesByTarget(v);
          foreach (var e in edges2)
          {
            if (!ReferenceEquals(e.Target, next))
            {
              // goto next
              e.Target = next;
              m++;
            }
          }
          m += OptimizationPass(g, next, visited);
        }
        else
        {
          foreach (var e in edges)
          {
            m += OptimizationPass(g, e.Target, visited);
          }
        }
      }
      return m;
    }

    private static List<CfNode> ToList(CfGraph g, CfNode v)
    {
      var list = new List<CfNode>();
      ToList(g, v, list, new HashSet<object>(new ReferenceEquality()));
      return list;
    }

    private static void ToList(CfGraph g, CfNode v, List<CfNode> list, HashSet<object> visit)
    {
      if (visit.Add(v))
      {
        if (!g.GetEdgesByTarget(v).Any())
        {
          list.Add(v);
        }
        var edges = g.GetEdgesBySource(v);
        foreach (var e in edges)
        {
          ToList(g, e.Target, list, visit);
        }
      }
    }
  }
}
