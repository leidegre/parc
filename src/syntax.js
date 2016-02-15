'use strict'

// abstract
function SyntaxNode() {
}

// a syntax node containing a single token
function SyntaxToken(token) {
  this.token_ = token
  Object.freeze(this)
}

// extends SyntaxNode
SyntaxToken.prototype = Object.create(SyntaxNode.prototype)
SyntaxToken.prototype.constructor = SyntaxToken

SyntaxToken.prototype.toString = function() {
  return this.token_.s_.toString()
}

// export SyntaxToken
module.exports.SyntaxToken = SyntaxToken

// a general "immutable" syntax node with children
function SyntaxTree(label, children) {
  this.label_ = label
  this.children_ = children
  Object.freeze(this)
}

// extends SyntaxNode
SyntaxTree.prototype = Object.create(SyntaxNode.prototype)
SyntaxTree.prototype.constructor = SyntaxTree

function indent(space, level) {
  var s = '\n'
  for (var i = 0, n = space * level; i < n; i++) {
    s += ' '
  }
  return (x) => 
    x.constructor === SyntaxTree ? s + x.toString(space, level) : x.toString()
}

SyntaxTree.prototype.toString = function(space, level) {
  if (space) {
    return `(${this.label_} ${this.children_.map(indent(space, (level || 0) + 1)).join(' ')})`
  } else {
    return `(${this.label_} ${this.children_.join(' ')})`
  }
}

// export SyntaxTree
module.exports.SyntaxTree = SyntaxTree