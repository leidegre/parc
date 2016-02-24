/*eslint strict: [2, "never"]*/
/*global window*/

// This file exports the particle API and when we bundle particle with browserify it will also export particle to the global window object which if we run in strict mode will not be possible. So this file is a bit special.

var Lexer = require('./lexer')
  , Parser = require('./parser')
  , Compiler = require('./compiler')
  , SubsetConstruction = require('./subset-construction')

function parse(input) {
  var lexer = new Lexer(input)
  lexer.load()
  var parser = new Parser(lexer)
  parser.parseGrammar()
  return parser.stack_[0]
}

function compile(input) {
  var syntax = parse(input)
  var compiler = new Compiler()
  syntax.accept(compiler)
  return {
    graph: compiler.graph_,
    root: compiler.root_
  }
}

function construct(input) {
  var compilationUnit = compile(input)
  var subsetConstruction = new SubsetConstruction(compilationUnit.graph)
  var dfa = subsetConstruction.construct(compilationUnit.root)
  return {
    dfa
  }
}

module.exports = {
  parse,
  compile,
  construct
}

// *sigh*
try {
  window.particle = module.exports
} catch (err) {
  // not a browser... 
}
