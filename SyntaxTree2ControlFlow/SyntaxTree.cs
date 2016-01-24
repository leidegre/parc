using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Parc
{
  public enum TokenKind
  {
    None,
    Literal,
    LeftBrace,
    RightBrace,
    LocalIdentifier,
    GlobalIdentifier,
    Pipe,
    Semicolon,
    EqualsSign,
    LeftParenthesis,
    RightParenthesis,
    Comma,
    Asterisk,
    QuestionMark,
    PlusSign,
    Hypen,
    Comment
  }

  public struct Token
  {
    public TokenKind Kind { get; }
    public string Value { get; }

    public Token(TokenKind kind, string value)
    {
      this.Kind = kind;
      this.Value = value;
    }
  }

  public abstract class Visitor
  {
    public abstract void Visit(SyntaxNode syntaxNode);
    public abstract void Visit(SyntaxToken syntaxToken);
  }

  public abstract class SyntaxTree
  {
    public static implicit operator SyntaxTree(Token token)
    {
      return new SyntaxToken(token);
    }

    public abstract void Accept(Visitor visitor);

    public override string ToString()
    {
      var sb = new StringBuilder();
      ToString(0, sb);
      return sb.ToString();
    }

    internal abstract void ToString(int indentation, StringBuilder sb);
  }

  public class SyntaxNode : SyntaxTree, IEnumerable<SyntaxTree>
  {
    public string Label { get; }

    public SyntaxTree this[int index] { get { return _subTree[index]; } }

    public int Count { get { return _subTree.Count; } }

    private List<SyntaxTree> _subTree;
    public bool HasSubTree
    {
      get { return _subTree != null && _subTree.Count > 0; }
    }

    public List<SyntaxTree> SubTree
    {
      get
      {
        if (_subTree == null)
        {
          _subTree = new List<SyntaxTree>();
        }
        return _subTree;
      }
      set
      {
        _subTree = value;
      }
    }

    public SyntaxNode(string label, params SyntaxTree[] subTree)
    {
      if (string.IsNullOrEmpty(label))
      {
        throw new ArgumentException();
      }
      this.Label = label;
      this.SubTree = subTree != null ? new List<SyntaxTree>(subTree) : null;
    }

    public override void Accept(Visitor visitor)
    {
      visitor.Visit(this);
    }

    internal override void ToString(int indentation, StringBuilder sb)
    {
      sb.Append('(');
      sb.Append(Label);
      if (HasSubTree)
      {
        if (SubTree.OfType<SyntaxToken>().SequenceEqual(SubTree))
        {
          for (int i = 0; i < SubTree.Count; i++)
          {
            sb.Append(' ');
            SubTree[i].ToString(indentation, sb);
          }
        }
        else
        {
          var nextIndentation = indentation + 1;
          for (int i = 0; i < SubTree.Count; i++)
          {
            sb.AppendLine();
            sb.Append(' ', nextIndentation);
            SubTree[i].ToString(nextIndentation, sb);
          }
          sb.AppendLine();
          sb.Append(' ', indentation);
        }
      }
      sb.Append(')');
    }

    public IEnumerator<SyntaxTree> GetEnumerator()
    {
      if (HasSubTree)
      {
        foreach (var syntaxNode in SubTree)
        {
          yield return syntaxNode;
        }
      }
    }

    IEnumerator IEnumerable.GetEnumerator()
    {
      return GetEnumerator();
    }
  }

  public class SyntaxToken : SyntaxTree
  {
    public Token Token { get; }

    public SyntaxToken(Token token)
    {
      this.Token = token;
    }

    public override void Accept(Visitor visitor)
    {
      visitor.Visit(this);
    }

    internal override void ToString(int indentation, StringBuilder sb)
    {
      sb.Append(CSharpUtility.ToLiteral(Token.Value));
    }
  }
}
