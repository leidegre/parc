using System;
using System.Collections.Generic;
using System.Linq;

namespace Parc
{
  class Binder : SyntaxVisitor
  {
    private readonly Compilation _unit;

    private readonly ListStack<CfNode> _controlFlow;

    public Binder(Compilation unit)
    {
      this._unit = unit;
      this._controlFlow = new ListStack<CfNode>();
    }

    protected override void VisitGrammarUnit(SyntaxNode grammarUnit)
    {
      foreach (var syntaxNode in grammarUnit)
      {
        syntaxNode.Accept(this);
      }
    }

    protected override void VisitNamespaceDeclaration(SyntaxNode namespaceDeclaration)
    {
      for (int i = 2; i < namespaceDeclaration.SubTree.Count - 1; i++)
      {
        namespaceDeclaration.SubTree[i].Accept(this);
      }
    }

    protected override void VisitProductionDeclaration(SyntaxNode productionDeclaration)
    {
      var identifier = (SyntaxToken)productionDeclaration.SubTree[0];

      var root = new CfNode(_unit.Graph) {
        Label = identifier.Token.Value,
      };

      for (int i = 2; i < productionDeclaration.SubTree.Count; i += 2)
      {
        _controlFlow.Push(root);
        productionDeclaration[i].Accept(this);
        var acceptingState = _controlFlow.Peek();
        acceptingState.Projection = new CfProjection(); // accept state
        _controlFlow.Pop();
        _controlFlow.Pop();
      }

      _unit.Graph.AddNode(root);
    }

    protected override void VisitImplicitToken(SyntaxNode implicitToken)
    {
      var prev = _controlFlow.Peek();
      var prevEdges = _unit.Graph.GetEdgesBySource(prev);

      var literalToken = (SyntaxToken)implicitToken.SubTree[0];
      var literal = literalToken.Token.Value;

      // we have to look for similar edges before we can put in our own
      var firstPrevEdge = prevEdges.FirstOrDefault(x => x.HasGuardSet && x.GuardSet.Contains(literal));
      if (firstPrevEdge != null)
      {
        System.Diagnostics.Debug.Assert(firstPrevEdge.GuardSet.Count == 1); // otherwise we need to split guard set
        _controlFlow.Push(firstPrevEdge.Target);
        return;
      }

      var next = new CfNode(_unit.Graph);

      _unit.Graph.AddEdge(new CfEdge {
        Source = prev,
        Target = next,
        GuardSet = {
          literal
        }
      });

      _controlFlow.Push(next);
    }

    protected override void VisitTokenReference(SyntaxNode tokenReference)
    {
      var prev = _controlFlow.Peek();
      var prevEdges = _unit.Graph.GetEdgesBySource(prev);

      var localIdentifierToken = (SyntaxToken)tokenReference.SubTree[0];
      var localIdentifier = localIdentifierToken.Token.Value;

      // we have to look for similar edges before we can put in our own
      var firstPrevEdge = prevEdges.FirstOrDefault(x => x.HasGuardSet && x.GuardSet.Contains(localIdentifier));
      if (firstPrevEdge != null)
      {
        System.Diagnostics.Debug.Assert(firstPrevEdge.GuardSet.Count == 1); // otherwise we need to split guard set
        _controlFlow.Push(firstPrevEdge.Target);
        return;
      }

      var next = new CfNode(_unit.Graph);

      _unit.Graph.AddEdge(new CfEdge {
        Source = prev,
        Target = next,
        GuardSet = {
          localIdentifier
        }
      });

      _controlFlow.Push(next);
    }

    protected override void VisitProductionReference(SyntaxNode productionReference)
    {
      var prev = _controlFlow.Peek();
      var prevEdges = _unit.Graph.GetEdgesBySource(prev);

      var gocalIdentifierToken = (SyntaxToken)productionReference.SubTree[0];
      var gocalIdentifier = gocalIdentifierToken.Token.Value;

      // we have to look for similar edges before we can put in our own
      var firstPrevEdge = prevEdges.FirstOrDefault(x => x.HasGuardSet && x.GuardSet.Contains(gocalIdentifier));
      if (firstPrevEdge != null)
      {
        System.Diagnostics.Debug.Assert(firstPrevEdge.GuardSet.Count == 1); // otherwise we need to split guard set
        _controlFlow.Push(firstPrevEdge.Target);
        return;
      }

      var next = new CfNode(_unit.Graph);

      _unit.Graph.AddEdge(new CfEdge {
        Source = prev,
        Target = next,
        GuardSet = {
          gocalIdentifier
        }
      });

      _controlFlow.Push(next);
    }

    protected override void VisitParenthesizedExpression(SyntaxNode syntaxNode)
    {
      syntaxNode[1].Accept(this);
    }

    protected override void VisitBlock(SyntaxNode syntaxNode)
    {
      using (var it = syntaxNode.GetEnumerator())
      {
        if (!it.MoveNext())
        {
          throw new InvalidOperationException("Empty block."); // concatenation
        }

        _controlFlow.Push(_controlFlow.Peek()); // dup

        do
        {
          it.Current.Accept(this);
          var top = _controlFlow.Peek();
          _controlFlow.Pop();
          _controlFlow.Pop();
          _controlFlow.Push(top);
        }
        while (it.MoveNext());
      }
    }

    protected override void VisitCase(SyntaxNode syntaxNode)
    {
      using (var it = syntaxNode.GetEnumerator())
      {
        if (!it.MoveNext())
        {
          throw new InvalidOperationException("Empty union.");
        }
        var enter = new CfNode(_unit.Graph);
        var leave = new CfNode(_unit.Graph);
        do
        {
          var syntax = it.Current;
          if (syntax is SyntaxToken)
          {
            continue;
          }
          var root = _controlFlow.Peek();
          var dummy = new CfNode(_unit.Graph);
          _controlFlow.Push(dummy);
          it.Current.Accept(this);
          _controlFlow.Bubble();
          var top = _controlFlow.Peek();
          _unit.Graph.AddEdge(new CfEdge {
            Source = root,
            Target = dummy,
          });
          _unit.Graph.AddEdge(new CfEdge {
            Source = top,
            Target = leave,
          });
          _controlFlow.Replace(root);
        }
        while (it.MoveNext());
        _controlFlow.Replace(leave);
      }
    }

    protected override void VisitQuantifier(SyntaxNode quantifier)
    {
      var syntax = (SyntaxNode)quantifier[0];
      var token = (SyntaxToken)quantifier[1];
      switch (token.Token.Kind)
      {
        case TokenKind.Asterisk:
          {
            KleenStar(syntax);
            break;
          }
        case TokenKind.PlusSign:
          {
            KleenStar(syntax, plus: true);
            break;
          }
        case TokenKind.QuestionMark:
          {
            var prev = _controlFlow.Peek();

            syntax.Accept(this);

            var curr = _controlFlow.Peek();

            var next = new CfNode(_unit.Graph);

            _unit.Graph.AddEdge(new CfEdge {
              Source = prev,
              Target = next,
            });

            _unit.Graph.AddEdge(new CfEdge {
              Source = curr,
              Target = next,
            });

            _controlFlow.Replace(next);
            break;
          }
        default:
          throw new NotSupportedException();
      }
    }

    private void KleenStar(SyntaxNode syntax, bool plus = false)
    {
      _controlFlow.Duplicate();
      var curr = _controlFlow.Peek();
      var enter = new CfNode(_unit.Graph);
      var leave = new CfNode(_unit.Graph);
      _controlFlow.Replace(enter);
      syntax.Accept(this);
      var star = _controlFlow.Peek();
      _controlFlow.Replace(leave);
      _unit.Graph.AddEdge(new CfEdge { Source = curr, Target = enter });
      if (!plus) _unit.Graph.AddEdge(new CfEdge { Source = curr, Target = leave });
      _unit.Graph.AddEdge(new CfEdge { Source = star, Target = enter, IsBackLink = true });
      _unit.Graph.AddEdge(new CfEdge { Source = star, Target = leave });
      _controlFlow.Bubble();
    }
  }
}