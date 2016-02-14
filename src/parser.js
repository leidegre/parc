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
  if (!this.err_ && this.lexer_.token_.type_ === type) {
    this.stack_.push(new SyntaxToken(this.lexer_.token_))
    this.lexer_.next()
    return true
  }
  return false
}

Parser.prototype.expect = function(type) {
  if (!this.accept(type)) {
    this.err_ = new Error(`Syntax error. Expected ${type} found ${this.lexer_.token_.type_}`)
  }
}

Parser.prototype.reduce = function(label, count) {
  if (count <= this.stack_.length) throw Error('Stack is not deep enough.')
  //
  var children = []
  for (var i = 0; i < count; i++) {
    children.push(this.stack_.pop())
  }
  this.stack_.push(new SyntaxTree(label, children))
}