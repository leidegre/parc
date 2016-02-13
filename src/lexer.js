"use strict"

var Token = require('./token')
  , tokenNext = require('./lexer-next')

var BAD_CHAR = -1

function isWhiteSpace(ch) {
  return (ch === 9) // tab
    || (ch === 10) // new line
    || (ch === 13) // carriage return
    || (ch === 32) // white space
}

function Lexer(inp) {
  if (typeof inp !== 'string') throw TypeError('input must be string')
  this.inp_ = inp
  this.head_ = 0
  this.tail_ = 0
}

// Decode a single character
// Note that this will return an UTF-16 code unit (not a Unicode code point!)
Lexer.prototype.load = function () {
  if (this.head_ < this.inp_.length) {
    this.ch_ = this.inp_.charCodeAt(this.head_)
  } else {
    this.ch_ = BAD_CHAR
  }
}

// Consume current character and load next
Lexer.prototype.read = function () {
  if (typeof this.ch_ === 'undefined') throw Error('must call load first')
  // >>>>>>>>
  this.head_++
}

Lexer.prototype.accept = function (ch) {
  if (arguments.length !== 1) throw Error('accept takes exactly 1 argument')
  if (typeof ch !== 'number') throw TypeError('character must be number')
  // >>>>>>>>
  if (this.ch_ === ch) {
    this.read()
    this.load()
    return true
  }
  return false
}

Lexer.prototype.acceptAny = function () {
  if (arguments.length !== 0) throw Error('acceptAny does not take any arguments')
  // >>>>>>>>
  if (this.ch_ !== BAD_CHAR) {
    this.read()
    this.load()
    return true
  }
  return false
}

Lexer.prototype.acceptWhiteSpace = function () {
  if (arguments.length !== 0) throw Error('acceptWhiteSpace does not take any arguments')
  // >>>>>>>>
  if (isWhiteSpace(this.ch_)) {
    this.read()
    this.load()
    return true
  }
  return false
}

Lexer.prototype.acceptBetween = function (lower, upper) {
  if (arguments.length !== 2) throw Error('acceptBetween takes exactly 2 arguments')
  if (typeof lower !== 'number') throw TypeError('lower must be number')
  if (typeof upper !== 'number') throw TypeError('upper must be number')
  // >>>>>>>>
  if ((lower <= this.ch_) && (this.ch_ <= upper)) {
    this.read()
    this.load()
    return true
  }
  return false
}

Lexer.prototype.yield = function (type) {
  this.token_ = new Token(undefined, type, Token.slice(this.inp_, this.tail_, this.head_), 1, 1)
  this.tail_ = this.head_
}

// import from lexer-next.js
Lexer.prototype.next = tokenNext

module.exports = Lexer