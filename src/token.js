"use strict"

// Slice
function Slice(s, startIndex, length) {
  this.s = s
  this.startIndex = startIndex
  this.length = length
}

Slice.prototype.toString = function() {
  return this.s.substr(this.startIndex, this.length)
}

// Token
function Token(leadingTrivia, type, text, lineNum, charPos) {
  this.leadingTrivia = leadingTrivia
  this.type = type
  this.text = text
  this.lineNum = lineNum
  this.charPos = charPos
}

module.exports = {
  Slice: Slice,
  Token: Token
}
