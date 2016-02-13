"use strict"

// import { Token } from './token'
let token = require('./token')
  , Token = token.Token

function Lexer(inp) {
  this.inp_ = inp
}

Lexer.prototype.read = function() {
  // Load the next character
}

Lexer.prototype.next = function() {
  
}
