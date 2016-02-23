'use strict'

const Token = require('./token')

// guards are a canonical representation of consuming some input, they are attached to edges in the control flow graph to tell us what kind/type of input is required to transition through a specific state in the lexer/parser stage. 

function Guard() {
}

// static
Guard.fromLiteral = function(not, x) {
  return new TokenGuard(not, x, x)
}

// static
Guard.fromRange = function(not, a, b) {
  return new TokenGuard(not, a, b)
}

function TokenGuard(not, a, b) {
  this.not_ = not
  this.a_ = getString(a.token_)
  this.b_ = a !== b ? getString(b.token_) : this.a_ // same
  this.lower_ = a
  this.upper_ = b
}

// TokenGuard extends Guard
TokenGuard.prototype = Object.create(Guard.prototype)
TokenGuard.prototype.constructor = TokenGuard

TokenGuard.prototype.overlaps = function(guard) {
  // [a, b] overlaps [x, y] if b >= x and a <= y
  const x = guard.a_
  const y = guard.b_
  return (this.b_.localeCompare(x) >= 0)
    && (this.a_.localeCompare(y) <= 0)
}

TokenGuard.prototype.equals = function(guard) {
  const x = guard.a_
  const y = guard.b_
  return (this.a_.localeCompare(x) == 0)
    && (this.b_.localeCompare(y) == 0)
}

function getString(token) {
  var s = token.s_.toString()
  return token.type_ == Token.TYPE.STRING_LITERAL ? Token.unescape(s) : s
}

TokenGuard.prototype.toJSON = function() {
  return {
    not: this.not_,
    lower: getString(this.lower_.token_),
    upper: getString(this.upper_.token_)
  }
}

TokenGuard.prototype.toString = function () {
  const lower = getString(this.lower_.token_)
  const upper = getString(this.upper_.token_)
  if (lower !== upper) {
    if (this.not_) {
      return `]${lower},${upper}[`  
    }
    return `[${lower},${upper}]`  
  }
  if (this.not_) {
    return `]${lower}[`  
  }
  return `[${lower}]`
}

module.exports = Guard