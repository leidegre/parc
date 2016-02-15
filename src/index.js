
var Lexer = require('./lexer')
  , Parser = require('./parser')
  
  function parse(input) {
    var lexer = new Lexer(input)
    lexer.load()
    var parser = new Parser(lexer)
    parser.parseGrammar()
    return parser.stack_[0]
  }
  
  module.exports.parse = parse