'use strict'

// see https://jsfiddle.net/2w57zoa4/

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

TokenGuard.prototype.sub = function(other) {
  
  // create a new guard that does not overlap with other
  
  if (other.a_.localeCompare(this.a_) <= 0
    && other.b_.localeCompare(this.b_) <= 0) {
    
  }
  
  // 1. does the interval though the lower half, return upper half
  // 2. does the interval though the upper half, return lower half
  // 3. is the extent of the interval greater than ours, return empty interval
  // 3. is the extent of the interval smaller than ours, return non-continuous
  
  return  
}

function getString(token) {
  var s = token.s_.toString()
  return token.type_ == Token.TYPE.STRING_LITERAL ? Token.unescape(s) : s
}

TokenGuard.prototype.toJSON = function() {
  return {
    not: this.not_,
    lower: this.a_,
    upper: this.b_
  }
}

TokenGuard.prototype.toString = function () {
  const a = this.a_
  const b = this.b_
  if (a !== b) {
    if (this.not_) {
      return `]${a},${b}[`  
    }
    return `[${a},${b}]`  
  }
  if (this.not_) {
    return `]${a}[`  
  }
  return `[${a}]`
}

module.exports = Guard