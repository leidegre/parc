'use strict'

var test = require('tape')
  , Lexer = require('./lexer')
  , Token = require('./token')
  
test('lex hello world', function (t) {
  t.plan(11)
  
  var lex = new Lexer('hello world')
  
  // an initial load is necessary to initialize the lexer
  lex.load() 
  
  t.ok(lex.accept('h'.charCodeAt(0)))
  t.ok(lex.accept('e'.charCodeAt(0)))
  t.ok(lex.accept('l'.charCodeAt(0)))
  t.ok(lex.accept('l'.charCodeAt(0)))
  t.ok(lex.accept('o'.charCodeAt(0)))
  
  t.ok(lex.acceptWhiteSpace())
  
  t.ok(lex.accept('w'.charCodeAt(0)))
  t.ok(lex.accept('o'.charCodeAt(0)))
  t.ok(lex.accept('r'.charCodeAt(0)))
  t.ok(lex.accept('l'.charCodeAt(0)))
  t.ok(lex.accept('d'.charCodeAt(0)))
})

test('lex string literal', function (t) {
  t.plan(3)
  
  var lex = new Lexer('"hello world"')
  
  lex.load() 
  
  t.ok(lex.next())
  
  t.ok(lex.token_)
  t.equal(lex.token_.s_.toString(), '"hello world"')
})

test('lex local', function (t) {
  t.plan(4)
  
  var lex = new Lexer('hello_world')
  
  lex.load() 
  
  t.ok(lex.next())
  
  t.ok(lex.token_)
  t.equal(lex.token_.type_, Token.TYPE.LOCAL)
  t.equal(lex.token_.s_.toString(), 'hello_world')
})

test('lex global', function (t) {
  t.plan(4)
  
  var lex = new Lexer('HelloWorld')
  
  lex.load() 
  
  t.ok(lex.next())
  
  t.ok(lex.token_)
  t.equal(lex.token_.type_, Token.TYPE.GLOBAL)
  t.equal(lex.token_.s_.toString(), 'HelloWorld')
})

test('lex many', function (t) {
  
  var tests = [
    { type: Token.TYPE.LEFT_PARENTHESIS, input: '(' },
    { type: Token.TYPE.RIGHT_PARENTHESIS, input: ')' },
    { type: Token.TYPE.ASTERISK, input: '*' },
    { type: Token.TYPE.PLUS_SIGN, input: '+' },
    { type: Token.TYPE.RANGE, input: '..' },
    { type: Token.TYPE.SEMICOLON, input: ';' },
    { type: Token.TYPE.EQUALS_SIGN, input: '=' },
    { type: Token.TYPE.QUESTION_MARK, input: '?' },
    { type: Token.TYPE.CIRCUMFLEX_ACCENT, input: '^' },
    { type: Token.TYPE.LEFT_BRACE, input: '{' },
    { type: Token.TYPE.PIPE, input: '|' },
    { type: Token.TYPE.RIGHT_BRACE, input: '}' },
  ]
  
  t.plan(2 * tests.length)
  
  tests.forEach((x) => {
    var lex = new Lexer(x.input)
    lex.load() 
    t.ok(lex.next(), `input ${x.input} is valid`)
    t.equal(lex.token_.type_, x.type, `token type is ${x.type}`)  
  })
  
})