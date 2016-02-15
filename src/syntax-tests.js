'use strict'

var test = require('tape')

var Token = require('./token')
  , syntax = require('./syntax')
  , SyntaxToken = syntax.SyntaxToken
  , SyntaxTree = syntax.SyntaxTree

function makeToken(s) {
  return new SyntaxToken(new Token(undefined, null, Token.slice(s, 0, s.length), 1, 1))
}

test('syntax linearization 1', function (t) {
  t.plan(1)
  var syntax_tree = new SyntaxTree('SyntaxTree', [makeToken('a')])
  t.equal(syntax_tree.toString(), '(SyntaxTree a)')
})

test('syntax linearization 2', function (t) {
  t.plan(1)
  var syntax_tree = new SyntaxTree('SyntaxTree', [makeToken('a'), makeToken('b')])
  t.equal(syntax_tree.toString(), '(SyntaxTree a b)')
})

test('syntax linearization 3', function (t) {
  t.plan(1)
  var syntax_tree = new SyntaxTree('SyntaxTree', [makeToken('a'), makeToken('b'), makeToken('c')])
  t.equal(syntax_tree.toString(), '(SyntaxTree a b c)')
})

test('syntax linearization 4', function (t) {
  t.plan(1)
  var syntax_tree = new SyntaxTree('SyntaxTree', [
    new SyntaxTree('SyntaxTree', [makeToken('a')]), 
    new SyntaxTree('SyntaxTree', [makeToken('b')])
  ])
  t.equal(syntax_tree.toString(), '(SyntaxTree (SyntaxTree a) (SyntaxTree b))')
})

test('syntax linearization 5', function (t) {
  t.plan(1)
  var syntax_tree = new SyntaxTree('SyntaxTree', [
    new SyntaxTree('SyntaxTree', [new SyntaxTree('SyntaxTree', [makeToken('a')])]), 
    new SyntaxTree('SyntaxTree', [makeToken('b')])
  ])
  t.equal(syntax_tree.toString(), '(SyntaxTree (SyntaxTree (SyntaxTree a)) (SyntaxTree b))')
})

test('syntax linearization 6', function (t) {
  t.plan(1)
  var syntax_tree = new SyntaxTree('SyntaxTree', [
    new SyntaxTree('SyntaxTree', [makeToken('a')]), 
    new SyntaxTree('SyntaxTree', [new SyntaxTree('SyntaxTree', [makeToken('b')])])
  ])
  t.equal(syntax_tree.toString(), '(SyntaxTree (SyntaxTree a) (SyntaxTree (SyntaxTree b)))')
})