
var fs = require('fs')
var Lexer = require('./src/lexer')

var argv = process.argv.slice(2)

switch (argv.shift()) {
  case 'lex': {
    var fn = argv.shift()
    var text = fs.readFileSync(fn, 'utf8')
    var lex = new Lexer(text)
    lex.load()
    while (lex.next()) {
      console.log(JSON.stringify(lex.token_))
    }
    if (lex.err_) {
      console.error(lex.err_)
    }
    break
  }
  default: {
    console.error('Unrecognized command-line option.')
  }
}