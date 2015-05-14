
#include "Lexer.h"

namespace {
template <int min, int max>
bool Between(int value) {
  static_assert(min <= max, "!(min <= max)");
  return (min <= value) & (value <= max);
}
bool IsWhiteSpace(int value) {
  return (value == '\t') | (value == '\n') | (value == '\r') | (value == ' ');
}
}

namespace parc {
Token Lexer::Next() {
  if (!inp_.Load()) {
    if (inp_.IsEmpty()) {
      return Token(Token::kEndOfFile);
    } else {
      return Token(Token::kError);
    }
  }
  Slice leading_trivia;
  while (IsWhiteSpace(inp_.Peek()) && inp_.Read()) {
    // yum...
  }
  leading_trivia = inp_.Accept();
  switch (inp_.Peek()) {
    case '"': {
      if (inp_.Read()) {
        while (inp_.Peek() != '"' && inp_.Read()) {
          // todo: escape sequence
        }
        if (inp_.Read()) {
          return Token(Token::kStringLiteral, inp_.Accept(), leading_trivia);
        }
      }
      return Token(Token::kError);
    }
    case '(': {
      inp_.Read();
      return Token(Token::kLeftParenthesis, inp_.Accept(), leading_trivia);
    }
    case ')': {
      inp_.Read();
      return Token(Token::kRightParenthesis, inp_.Accept(), leading_trivia);
    }
    case '*': {
      inp_.Read();
      return Token(Token::kAsterisk, inp_.Accept(), leading_trivia);
    }
    case '+': {
      inp_.Read();
      return Token(Token::kPlusSign, inp_.Accept(), leading_trivia);
    }
    case ';': {
      inp_.Read();
      return Token(Token::kSemicolon, inp_.Accept(), leading_trivia);
    }
    case '=': {
      inp_.Read();
      return Token(Token::kEqualsSign, inp_.Accept(), leading_trivia);
    }
    case '?': {
      inp_.Read();
      return Token(Token::kQuestionMark, inp_.Accept(), leading_trivia);
    }
    case '{': {
      inp_.Read();
      return Token(Token::kLeftBrace, inp_.Accept(), leading_trivia);
    }
    case '|': {
      inp_.Read();
      return Token(Token::kPipe, inp_.Accept(), leading_trivia);
    }
    case '}': {
      inp_.Read();
      return Token(Token::kRightBrace, inp_.Accept(), leading_trivia);
    }
    default: {
      // Global (production)
      if (Between<'A', 'Z'>(inp_.Peek())) {
        while (inp_.Read() && (Between<'0', '9'>(inp_.Peek()) |
                               Between<'A', 'Z'>(inp_.Peek()) |
                               Between<'a', 'z'>(inp_.Peek()))) {
          // yum...
        }
        return Token(Token::kGlobal, inp_.Accept(), leading_trivia);
      }
      // Local (token)
      if (Between<'a', 'z'>(inp_.Peek())) {
        while (inp_.Read() && (Between<'0', '9'>(inp_.Peek()) |
                               Between<'A', 'Z'>(inp_.Peek()) |
                               Between<'a', 'z'>(inp_.Peek()))) {
          // yum...
        }
        return Token(Token::kLocal, inp_.Accept(), leading_trivia);
      }
      break;
    }
  }
  return Token(Token::kError);
}
}
