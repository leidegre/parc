using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Parc
{
  class Lexer
  {
    struct Reader
    {
      private char[] _cs;
      private int _position;
      public int Position { get { return _position; } }

      public bool IsEmpty { get { return !(_position < _cs.Length); } }

      public Reader(char[] cs)
      {
        this._cs = cs;
        this._position = 0;
      }

      // REQUIRES: !IsEmpty
      public char Peek()
      {
        return _cs[_position];
      }

      public bool Peek(Func<char, bool> predicate)
      {
        var p0 = _position;
        if (p0 < _cs.Length)
        {
          return predicate(_cs[p0]);
        }
        return false;
      }

      // true if the next character is same as ch0
      public bool Peek(char ch0)
      {
        var p0 = _position;
        if (p0 < _cs.Length)
        {
          return (_cs[p0] == ch0);
        }
        return false;
      }

      // true if the next character is same as ch0 and the character following is same as ch1
      public bool Peek(char ch0, char ch1)
      {
        var p0 = _position;
        var p1 = _position + 1;
        if (p1 < _cs.Length)
        {
          return (_cs[p0] == ch0) & (_cs[p1] == ch1);
        }
        return false;
      }

      public bool Read()
      {
        return _position++ < _cs.Length;
      }

      public string ToString(int startPosition)
      {
        return new string(_cs, startPosition, _position - startPosition);
      }
    }

    private Reader _r;

    private bool Between(char lower, char upper)
    {
      var ch = _r.Peek();
      if ((lower <= ch) & (ch <= upper))
      {
        return true;
      }
      return false;
    }

    public Lexer(string inp)
    {
      _r = new Reader(inp.ToCharArray());
    }

    public Token Token { get; private set; }

    public bool Read()
    {
      while (_r.Peek(char.IsWhiteSpace))
      {
        _r.Read();
      }
      if (_r.IsEmpty)
      {
        Token = default(Token);
        return false;
      }
      var startPosition = _r.Position;
      var ch = _r.Peek();
      switch (ch)
      {
        case '\"':
          {
            _r.Read();
            while (_r.Peek() != '\"')
            {
              _r.Read();
            }
            _r.Read();
            Token = new Token(TokenKind.Literal, _r.ToString(startPosition));
            return true;
          }
        case '-':
          {
            _r.Read();
            if (_r.Peek() == '-')
            {
              _r.Read();
              while (((_r.Peek() != '\r') & (_r.Peek() != '\n')) && _r.Read())
              {
                ;
              }
              if (_r.Peek('\r', '\n'))
              {
                _r.Read();
                _r.Read();
              }
              else
              {
                _r.Read();
              }
              Token = new Token(TokenKind.Comment, _r.ToString(startPosition));
              return true;
            }
            Token = new Token(TokenKind.Hypen, _r.ToString(startPosition));
            return true;
          }
        case '{':
          {
            _r.Read();
            Token = new Token(TokenKind.LeftBrace, _r.ToString(startPosition));
            return true;
          }
        case '}':
          {
            _r.Read();
            Token = new Token(TokenKind.RightBrace, _r.ToString(startPosition));
            return true;
          }
        case '(':
          {
            _r.Read();
            Token = new Token(TokenKind.LeftParenthesis, _r.ToString(startPosition));
            return true;
          }
        case ')':
          {
            _r.Read();
            Token = new Token(TokenKind.RightParenthesis, _r.ToString(startPosition));
            return true;
          }
        case '|':
          {
            _r.Read();
            Token = new Token(TokenKind.Pipe, _r.ToString(startPosition));
            return true;
          }
        case ',':
          {
            _r.Read();
            Token = new Token(TokenKind.Comma, _r.ToString(startPosition));
            return true;
          }
        case ';':
          {
            _r.Read();
            Token = new Token(TokenKind.Semicolon, _r.ToString(startPosition));
            return true;
          }
        case '=':
          {
            _r.Read();
            Token = new Token(TokenKind.EqualsSign, _r.ToString(startPosition));
            return true;
          }
        case '*':
          {
            _r.Read();
            Token = new Token(TokenKind.Asterisk, _r.ToString(startPosition));
            return true;
          }
        case '?':
          {
            _r.Read();
            Token = new Token(TokenKind.QuestionMark, _r.ToString(startPosition));
            return true;
          }
        case '+':
          {
            _r.Read();
            Token = new Token(TokenKind.PlusSign, _r.ToString(startPosition));
            return true;
          }
        default:
          {
            if (Between('a', 'z'))
            {
              _r.Read();
              while (_r.Peek() == '_' || Between('0', '9') || Between('A', 'Z') || Between('a', 'z'))
              {
                _r.Read();
              }
              Token = new Token(TokenKind.LocalIdentifier, _r.ToString(startPosition));
              return true;
            }
            if (Between('A', 'Z'))
            {
              _r.Read();
              while (Between('0', '9') || Between('A', 'Z') || Between('a', 'z'))
              {
                _r.Read();
              }
              Token = new Token(TokenKind.GlobalIdentifier, _r.ToString(startPosition));
              return true;
            }
            return false;
          }
      }
    }
  }
}
