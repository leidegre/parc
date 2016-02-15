'use strict'

var Token = require('./token')
  , syntax = require('./syntax')
  , SyntaxToken = syntax.SyntaxToken
  , SyntaxTree = syntax.SyntaxTree

function Parser(lexer) {
  this.lexer_ = lexer
  this.lexer_.next() // initialize
  this.stack_ = []
}

Parser.prototype.accept = function (type) {
  if (typeof type !== 'string') throw new TypeError('type must be string')
  //
  if (!this.err_ && this.lexer_.token_.type_ === type) {
    this.stack_.push(new SyntaxToken(this.lexer_.token_))
    this.lexer_.next()
    return true
  }
  return false
}

Parser.prototype.expect = function (type) {
  if (!this.accept(type)) {
    this.err_ = new Error(`Syntax error. Expected ${type} found ${this.lexer_.token_.type_}`)
  }
}

Parser.prototype.reduce = function (label, count) {
  if (!(count <= this.stack_.length)) throw Error(`Stack (${this.stack_.length}) is not large enough (${count}).`)
  //
  var children = []
  for (var i = 0; i < count; i++) {
    children.push(this.stack_.pop())
  }
  children.reverse()
  this.stack_.push(new SyntaxTree(label, children))
}

//
Parser.prototype.parseGrammar = function () {
  var c = 0
  if (this.parseNamespace()) {
    c++
    while (this.parseNamespace()) {
      c++
    }
    this.reduce('Grammar', c)
    return true
  }
  return false
}

Parser.prototype.parseNamespace = function () {
  var c = 0
  if (this.accept(Token.TYPE.STRING_LITERAL)) {
    c++
    this.expect(Token.TYPE.LEFT_BRACE)
    c++
    while (this.parseRule()) {
      c++
    }
    this.expect(Token.TYPE.RIGHT_BRACE)
    c++
    this.reduce('Namespace', c)
    return true
  }
  return false
}

Parser.prototype.parseRule = function () {
  if (this.parseOption()) {
    return true
  }
  if (this.parseTokenRule()) {
    return true
  }
  // if (this.parseProductionRule()) {
  //   return true
  // }
  return false
}

Parser.prototype.parseOption = function () {
  var c = 0
  if (this.accept(Token.TYPE.STRING_LITERAL)) {
    c++
    this.expect(Token.TYPE.STRING_LITERAL)
    c++
    this.expect(Token.TYPE.SEMICOLON)
    c++
    this.reduce('Option', c)
    return true
  }
  return false
}

Parser.prototype.parseTokenRule = function () {
  var c = 0
  if (this.accept(Token.TYPE.LOCAL)) {
    c++
    this.expect(Token.TYPE.EQUALS_SIGN)
    c++
    // TokenExpr+
    if (!this.parseTokenRep()) {
      this.err_ = new Error('Expected TokenRep')
      return false
    }
    c++
    while (this.parseTokenRep()) {
      c++
    }
    this.expect(Token.TYPE.SEMICOLON)
    c++
    this.reduce('Token', c)
    return true
  }
  return false
}

Parser.prototype.parseTokenPrimary = function () {
  var c = 0
  if (this.accept(Token.TYPE.STRING_LITERAL)) {
    c++
    this.reduce('Literal', c)
    return true
  }
  // unary prefix operator
  if (this.accept(Token.TYPE.CIRCUMFLEX_ACCENT)) {
    c++
    if (!this.parseTokenExpr()) {
      this.err_ = new Error('Syntax error. Expected production TokenExpr.')
      return false
    }
    c++
    this.reduce('Negation', c)
    return true
  }
  if (this.accept(Token.TYPE.LEFT_PARENTHESIS)) {
    c++
    if (!this.parseTokenExpr()) {
      this.err_ = new Error('Syntax error. Expected production TokenExpr.')
      return false
    }
    c++
    this.expect(Token.TYPE.RIGHT_PARENTHESIS)
    c++
    this.reduce('Eval', c)
    return true
  }
  return false
}

Parser.prototype.parseTokenExpr = function () {
  var c = 0
  if (this.parseTokenPrimary()) {
    c++
    // binary operator
    if (this.accept(Token.TYPE.PIPE)) {
      c++
      if (!this.parseTokenExpr()) {
        this.err_ = new Error('Syntax error. Expected production TokenExpr.')
        return false
      }
      c++
      this.reduce('Choice', c)
      return true
    }
    if (this.accept(Token.TYPE.RANGE)) {
      c++
      if (!this.parseTokenExpr()) {
        this.err_ = new Error('Syntax error. Expected production TokenExpr.')
        return false
      }
      c++
      this.reduce('Range', c)
      return true
    }
    return true
  }
  return false
}

Parser.prototype.parseTokenRep = function () {
  var c = 0
  if (this.parseTokenExpr()) {
    c++
    // unary suffix operator
    if (this.accept(Token.TYPE.ASTERISK)
    || this.accept(Token.TYPE.PLUS_SIGN)
    || this.accept(Token.TYPE.QUESTION_MARK)) {
      c++
      this.reduce('Quantifier', c)
      return true
    }
    return true
  }
  return false
}

module.exports = Parser