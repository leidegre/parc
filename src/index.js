'use strict'

var Lexer = require('./lexer')
  , Parser = require('./parser')
  , Compiler = require('./compiler')

function parse(input) {
  var lexer = new Lexer(input)
  lexer.load()
  var parser = new Parser(lexer)
  parser.parseGrammar()
  return parser.stack_[0]
}

function compile(input) {
  const syntax = parse(input)
  var compiler = new Compiler()
  syntax.accept(compiler)
  return compiler.graph_
}
  
module.exports = {
  parse,
  compile
}

// browserify
if (window) {
  window.particle = module.exports
}