using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Parc
{
  class SyntaxVisitor : Visitor
  {
    public override void Visit(SyntaxToken syntaxToken)
    {
      throw new NotImplementedException();
    }

    public override void Visit(SyntaxNode syntaxNode)
    {
      switch (syntaxNode.Label)
      {
        case "GrammarUnit":
          VisitGrammarUnit(syntaxNode);
          break;
        case "NamespaceDeclaration":
          VisitNamespaceDeclaration(syntaxNode);
          break;
        case "ProductionDeclaration":
          VisitProductionDeclaration(syntaxNode);
          break;
        case "ProductionBody":
          VisitProductionBody(syntaxNode);
          break;
        case "ImplicitToken":
          VisitImplicitToken(syntaxNode);
          break;
        case "TokenReference":
          VisitTokenReference(syntaxNode);
          break;
        case "ProductionReference":
          VisitProductionReference(syntaxNode);
          break;
        case "Parentheses":
          VisitParenthesizedExpression(syntaxNode);
          break;
        case "Block":
          VisitBlock(syntaxNode);
          break;
        case "Case":
          VisitCase(syntaxNode);
          break;
        case "Quantifier":
          VisitQuantifier(syntaxNode);
          break;
        default:
          throw new InvalidOperationException();
      }
    }

    protected virtual void VisitQuantifier(SyntaxNode quantifier)
    {
      throw new NotImplementedException();
    }

    protected virtual void VisitGrammarUnit(SyntaxNode grammarUnit)
    {
      throw new NotImplementedException();
    }

    protected virtual void VisitNamespaceDeclaration(SyntaxNode namespaceDeclaration)
    {
      throw new NotImplementedException();
    }

    protected virtual void VisitProductionDeclaration(SyntaxNode productionDeclaration)
    {
      throw new NotImplementedException();
    }

    protected virtual void VisitProductionBody(SyntaxNode projectionStatement)
    {
      throw new NotImplementedException();
    }

    protected virtual void VisitImplicitToken(SyntaxNode syntaxNode)
    {
      throw new NotImplementedException();
    }

    protected virtual void VisitTokenReference(SyntaxNode tokenReference)
    {
      throw new NotImplementedException();
    }

    protected virtual void VisitProductionReference(SyntaxNode productionReference)
    {
      throw new NotImplementedException();
    }

    protected virtual void VisitParenthesizedExpression(SyntaxNode eval)
    {
      throw new NotImplementedException();
    }

    protected virtual void VisitBlock(SyntaxNode block)
    {
      throw new NotImplementedException();
    }

    protected virtual void VisitCase(SyntaxNode @case)
    {
      throw new NotImplementedException();
    }
  }
}
