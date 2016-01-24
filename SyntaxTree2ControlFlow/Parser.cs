using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Parc
{
  class Parser
  {
    private readonly Lexer _lexer;
    private readonly Stack<SyntaxTree> _syntax;

    public SyntaxTree SyntaxTree { get { return _syntax.Peek(); } }

    public Parser(Lexer lexer)
    {
      _lexer = lexer;
      _syntax = new Stack<SyntaxTree>();
      Read();
    }

    public bool ParseGrammarUnit()
    {
      int c = 0;
      if (ParseNamespaceDeclaration())
      {
        c++;
        while (ParseNamespaceDeclaration())
        {
          c++;
        }
        Reduce(c, "GrammarUnit");
        return true;
      }
      return false;
    }

    public bool ParseNamespaceDeclaration()
    {
      int c = 0;
      if (Accept(TokenKind.Literal))
      {
        c++;

        Expect(TokenKind.LeftBrace);
        c++;

        while (ParseRule())
        {
          c++;
        }

        Expect(TokenKind.RightBrace);
        c++;

        Reduce(c, "NamespaceDeclaration");
        return true;
      }
      return false;
    }

    private bool ParseRule()
    {
      if (ParseProductionDeclaration())
      {
        return true;
      }
      return false;
    }

    private bool ParseProductionDeclaration()
    {
      int c = 0;
      if (Accept(TokenKind.GlobalIdentifier))
      {
        c++;

        Expect(TokenKind.EqualsSign);
        c++;

        for (; ;)
        {
          if (ParseProduction())
          {
            c++;
            if (Accept(TokenKind.Comma))
            {
              c++;
              continue;
            }
          }
          break;
        }

        Expect(TokenKind.Semicolon);
        c++;

        Reduce(c, "ProductionDeclaration");
        return true;
      }
      return false;
    }

    private bool ParseProductionBody()
    {
      int c = 0;
      if (ParseProduction())
      {
        c++;
        Reduce(c, "ProductionBody");
        return true;
      }
      return false;
    }

    private bool ParseProduction()
    {
      int c = 0;
      if (ParsePrimaryProduction())
      {
        c++;
        if (Accept(TokenKind.Pipe))
        {
          do
          {
            c++;
            System.Diagnostics.Trace.Assert(ParsePrimaryProduction());
            c++;
          }
          while (Accept(TokenKind.Pipe));
          Reduce(c, "Case");
          return true;
        }
        if (ParsePrimaryProduction())
        {
          do
          {
            c++;
          }
          while (ParsePrimaryProduction());
          Reduce(c, "Block");
          return true;
        }
        return true;
      }
      return false;
    }

    private bool ParsePrimaryProduction()
    {
      int c = 0;
      if (Accept(TokenKind.Literal))
      {
        c++;
        Reduce(c, "ImplicitToken");
        ParseQuantifier();
        return true;
      }
      if (Accept(TokenKind.LocalIdentifier))
      {
        c++;
        Reduce(c, "TokenReference");
        ParseQuantifier();
        return true;
      }
      if (Accept(TokenKind.GlobalIdentifier))
      {
        c++;
        Reduce(c, "ProductionReference");
        ParseQuantifier();
        return true;
      }
      if (Accept(TokenKind.LeftParenthesis))
      {
        c++;
        System.Diagnostics.Trace.Assert(ParseProduction());
        c++;
        Expect(TokenKind.RightParenthesis);
        c++;
        Reduce(c, "Parentheses");
        ParseQuantifier();
        return true;
      }
      return false;
    }

    private bool ParseQuantifier()
    {
      int c = 1;
      if (Accept(TokenKind.Asterisk))
      {
        c++;
        Reduce(c, "Quantifier");
        return true;
      }
      if (Accept(TokenKind.PlusSign))
      {
        c++;
        Reduce(c, "Quantifier"); // special case: a+ = a a*
        return true;
      }
      if (Accept(TokenKind.QuestionMark))
      {
        c++;
        Reduce(c, "Quantifier");
        return true;
      }
      return false;
    }

    #region Utility

    private void Read()
    {
      do
      {
        _lexer.Read();
      }
      while (_lexer.Token.Kind == TokenKind.Comment);
    }

    private bool Accept(TokenKind kind)
    {
      var token = _lexer.Token;
      if (token.Kind == kind)
      {
        Read();
        _syntax.Push(token);
        return true;
      }
      return false;
    }

    private void Expect(TokenKind kind)
    {
      if (!Accept(kind))
      {
        throw new InvalidOperationException("Parser error: expected '" + kind + "' found '" + _lexer.Token.Kind + "'.");
      }
    }

    private void Reduce(int count, string label)
    {
      if (!(count > 0))
      {
        throw new ArgumentOutOfRangeException();
      }
      var children = new SyntaxTree[count];
      for (int i = 0; i < count; i++)
      {
        children[count - (1 + i)] = _syntax.Pop();
      }
      _syntax.Push(new SyntaxNode(label, children));
    }

    #endregion
  }
}
