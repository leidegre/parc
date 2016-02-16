'use strict'

// Slice
function Slice(s, begin, end) {
  this.s_ = s
  this.begin_ = begin
  this.end_ = end
}

Object.defineProperty(Slice.prototype, 'length', {
  get: function () {
    return this.end_ - this.begin_
  }
})

Slice.prototype.toString = function () {
  return this.s_.substring(this.begin_, this.end_)
}

// JSON.stringify
Slice.prototype.toJSON = function () {
  return this.toString()
}

// Token
function Token(leading_trivia, type, s, line_num, char_pos) {
  this.leading_trivia_ = leading_trivia
  this.type_ = type
  this.s_ = s
  this.line_num_ = line_num
  this.char_pos_ = char_pos
}

// static member
Token.slice = function (s, begin, end) {
  return new Slice(s, begin, end)
}

// static member
Token.TYPE = Object.freeze({
  WHITE_SPACE: 'WHITE_SPACE',
  STRING_LITERAL: 'STRING_LITERAL',
  LEFT_PARENTHESIS: 'LEFT_PARENTHESIS',
  RIGHT_PARENTHESIS: 'RIGHT_PARENTHESIS',
  ASTERISK: 'ASTERISK',
  PLUS_SIGN: 'PLUS_SIGN',
  RANGE: 'RANGE',
  SEMICOLON: 'SEMICOLON',
  EQUALS_SIGN: 'EQUALS_SIGN',
  QUESTION_MARK: 'QUESTION_MARK',
  CIRCUMFLEX_ACCENT: 'CIRCUMFLEX_ACCENT',
  LEFT_BRACE: 'LEFT_BRACE',
  PIPE: 'PIPE',
  RIGHT_BRACE: 'RIGHT_BRACE',
  LOCAL: 'LOCAL',
  GLOBAL: 'GLOBAL',
})

module.exports = Token
