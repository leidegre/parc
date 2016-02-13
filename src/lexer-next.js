'use strict'

var Token = require('./token')

function nextString(lexer) {
  if (!lexer.accept('"'.charCodeAt(0))) {
    // note: this is not an error
    return false
  }
  for (; ;) {
    if (lexer.accept('\\'.charCodeAt(0))) {
      // escape sequence
      switch (lexer.ch_) {
        case '0'.charCodeAt(0):
        case 'n'.charCodeAt(0):  // we don't need \r if we normalize new-line, \r\n -> \n
        case 't'.charCodeAt(0):
        case '"'.charCodeAt(0):
        case '\\'.charCodeAt(0): {
          lexer.acceptAny()
          break
        }
        case 'x'.charCodeAt(0): {
          lexer.acceptAny()
          if (!(lexer.acceptBetween('0'.charCodeAt(0), '9'.charCodeAt(0)) ||
            lexer.acceptBetween('A'.charCodeAt(0), 'F'.charCodeAt(0)) ||
            lexer.acceptBetween('a'.charCodeAt(0), 'f'.charCodeAt(0)))) {
            // todo: set error state
            return false
          }
          if (!(lexer.acceptBetween('0'.charCodeAt(0), '9'.charCodeAt(0)) ||
            lexer.acceptBetween('A'.charCodeAt(0), 'F'.charCodeAt(0)) ||
            lexer.acceptBetween('a'.charCodeAt(0), 'f'.charCodeAt(0)))) {
            // todo: set error state
            return false
          }
          break
        }
        default: {
          // todo: set error state (unrecognized escape sequence)
          return false
        }
      }
    }
    if (lexer.accept('"'.charCodeAt(0))) {
      break
    }
    // note the following line will accept new-line
    if (!lexer.acceptAny()) {
      // todo: set error state
      return false
    }
  }
  return true
}

function nextLocal(lexer) {
  if (!lexer.acceptBetween('a'.charCodeAt(0), 'z'.charCodeAt(0))) {
    return false
  }
  while (lexer.acceptBetween('0'.charCodeAt(0), '9'.charCodeAt(0)) ||
    lexer.accept('_'.charCodeAt(0)) ||
    lexer.acceptBetween('a'.charCodeAt(0), 'z'.charCodeAt(0))) {
    // nom nom nom...
  }
  return true
}

function nextGlobal(lexer) {
  if (!lexer.acceptBetween('A'.charCodeAt(0), 'Z'.charCodeAt(0))) {
    return false
  }
  while (lexer.acceptBetween('0'.charCodeAt(0), '9'.charCodeAt(0)) ||
    lexer.acceptBetween('A'.charCodeAt(0), 'Z'.charCodeAt(0)) ||
    lexer.acceptBetween('a'.charCodeAt(0), 'z'.charCodeAt(0))) {
    // nom nom nom...
  }
  return true
}

function next() {
  if (this.acceptWhiteSpace()) {
    while (this.acceptWhiteSpace()) {
      // nom nom nom...
    }
    this.yield(Token.TYPE.WHITE_SPACE)
    // we don't return here and instead put white space in an intermediate token
    this.leading_trivia_ = this.token_
  }
  switch (this.ch_) {
    case '"'.charCodeAt(0): {
      if (nextString(this)) {
        this.yield(Token.TYPE.STRING_LITERAL)
        return true
      }
      // todo: set error state
      this.err_ = new Error('Unrecognized input')
      return false
    }
    case '('.charCodeAt(0): {
      this.acceptAny()
      this.yield(Token.TYPE.LEFT_PARENTHESIS)
      return true
    }
    case ')'.charCodeAt(0): {
      this.acceptAny()
      this.yield(Token.TYPE.RIGHT_PARENTHESIS)
      return true
    }
    case '*'.charCodeAt(0): {
      this.acceptAny()
      this.yield(Token.TYPE.ASTERISK)
      return true
    }
    case '+'.charCodeAt(0): {
      this.acceptAny()
      this.yield(Token.TYPE.PLUS_SIGN)
      return true
    }
    case '.'.charCodeAt(0): {
      this.acceptAny()
      if (this.accept('.'.charCodeAt(0))) {
        this.yield(Token.TYPE.RANGE)
        return true
      }
      // todo: set error state
      this.err_ = new Error('Unrecognized input')
      return false
    }
    case ';'.charCodeAt(0): {
      this.acceptAny()
      this.yield(Token.TYPE.SEMICOLON)
      return true
    }    
    case '='.charCodeAt(0): {
      this.acceptAny()
      this.yield(Token.TYPE.EQUALS_SIGN)
      return true
    }
    case '?'.charCodeAt(0): {
      this.acceptAny()
      this.yield(Token.TYPE.QUESTION_MARK)
      return true
    }
    case '^'.charCodeAt(0): {
      this.acceptAny()
      this.yield(Token.TYPE.CIRCUMFLEX_ACCENT)
      return true
    }
    case '{'.charCodeAt(0): {
      this.acceptAny()
      this.yield(Token.TYPE.LEFT_BRACE)
      return true
    }
    case '|'.charCodeAt(0): {
      this.acceptAny()
      this.yield(Token.TYPE.PIPE)
      return true
    }
    case '}'.charCodeAt(0): {
      this.acceptAny()
      this.yield(Token.TYPE.RIGHT_BRACE)
      return true
    }
    default: {
      if (('a'.charCodeAt(0) <= this.ch_) && (this.ch_ <= 'z'.charCodeAt(0))) {
        if (nextLocal(this)) {
          this.yield(Token.TYPE.LOCAL)
          return true
        }
      }
      if (('A'.charCodeAt(0) <= this.ch_) && (this.ch_ <= 'Z'.charCodeAt(0))) {
        if (nextGlobal(this)) {
          this.yield(Token.TYPE.GLOBAL)
          return true
        }
      }
      break
    }
  }
  this.err_ = new Error('Unrecognized input')
  return false
}

module.exports = next