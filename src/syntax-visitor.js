'use strict'

var syntax = require('./syntax')
  , SyntaxTree = syntax.SyntaxTree

function SyntaxVisitor() {
}

SyntaxVisitor.prototype.__accept__ = function(node) {
  if (node.constructor !== SyntaxTree) throw new TypeError('node must be SyntaxTree')
  this[node.label_](node)
}

SyntaxVisitor.prototype.Grammar = function(node) {
  console.log('visiting Grammar node')
  for (var i = 0; i < node.children_.length; i++) {
    this.__accept__(node.children_[i])
  }
}

SyntaxVisitor.prototype.Namespace = function(node) {
  console.log('visiting Namespace node')
  for (var i = 2, end = node.children_.length - 1; i < end; i++) {
    this.__accept__(node.children_[i])
  }
}

SyntaxVisitor.prototype.Token = function(node) {
  console.log('visiting Token node')
  for (var i = 2, end = node.children_.length - 1; i < end; i++) {
    this.__accept__(node.children_[i])
  }
}

SyntaxVisitor.prototype.Literal = function(node) {
  console.log('visiting Literal node')
}

SyntaxVisitor.prototype.Negation = function(node) {
  console.log('visiting Negation node')
  this.__accept__(node.children_[1])
}

SyntaxVisitor.prototype.Quantifier = function(node) {
  console.log('visiting Quantifier node')
  this.__accept__(node.children_[0])
}

module.exports = SyntaxVisitor