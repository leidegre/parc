'use strict'

const Token = require('./token')

// guards are a canonical representation of consuming some input, they are attached to edges in the control flow graph to tell us what kind/type of input is required to transition through a specific state in the lexer/parser stage. 

function Guard() {
  // todo: overlap test
}

// static
Guard.fromLiteral = function(not, literal) {
  return new TokenGuard(not, literal, literal)
}

// static
Guard.fromRange = function(not, lower, upper) {
  return new TokenGuard(not, lower, upper)
}

function TokenGuard(not, lower, upper) {
  this.not_ = not
  this.lower_ = lower
  this.upper_ = upper
}

// TokenGuard extends Guard
TokenGuard.prototype = Object.create(Guard.prototype)
TokenGuard.prototype.constructor = TokenGuard

function toGuardString(token) {
  var s = token.s_.toString()
  return token.type_ == Token.TYPE.STRING_LITERAL ? Token.unescape(s) : s
}

TokenGuard.prototype.toJSON = function() {
  return {
    not: this.not_,
    lower: toGuardString(this.lower_.token_),
    upper: toGuardString(this.upper_.token_)
  }
}

module.exports = Guard