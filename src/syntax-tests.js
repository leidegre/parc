'use strict'

var test = require('tape')

var Token = require('./token')
  , syntax = require('./syntax')
  , SyntaxToken = syntax.SyntaxToken
  , SyntaxTree = syntax.SyntaxTree

function makeToken(s) {
  return new SyntaxToken(new Token(undefined, null, Token.slice(s, 0, s.length), 1, 1))
}

test('syntax pretty print 1', function (t) {
  var syntax_tree = new SyntaxTree('SyntaxTree', [makeToken('a')])
  console.log(syntax_tree.toString())
  t.end()
})

test('syntax pretty print 2', function (t) {
  var syntax_tree = new SyntaxTree('SyntaxTree', [makeToken('a'), makeToken('b')])
  console.log(syntax_tree.toString())
  t.end()
})

test('syntax pretty print 3', function (t) {
  var syntax_tree = new SyntaxTree('SyntaxTree', [makeToken('a'), makeToken('b'), makeToken('c')])
  console.log(syntax_tree.toString())
  t.end()
})

test('syntax pretty print 4', function (t) {
  var syntax_tree = new SyntaxTree('SyntaxTree', [
    new SyntaxTree('SyntaxTree', [makeToken('a')]), 
    new SyntaxTree('SyntaxTree', [makeToken('b')])
  ])
  console.log(syntax_tree.toString())
  t.end()
})

test('syntax pretty print 5', function (t) {
  var syntax_tree = new SyntaxTree('SyntaxTree', [
    new SyntaxTree('SyntaxTree', [new SyntaxTree('SyntaxTree', [makeToken('a')])]), 
    new SyntaxTree('SyntaxTree', [makeToken('b')])
  ])
  console.log(syntax_tree.toString())
  t.end()
})

test('syntax pretty print 6', function (t) {
  var syntax_tree = new SyntaxTree('SyntaxTree', [
    new SyntaxTree('SyntaxTree', [makeToken('a')]), 
    new SyntaxTree('SyntaxTree', [new SyntaxTree('SyntaxTree', [makeToken('b')])])
  ])
  console.log(syntax_tree.toString())
  t.end()
})