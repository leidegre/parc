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

SyntaxTree.prototype.toString = function(indentation) {
  if (this.children_.length > 1) {
    indentation = (indentation || '') + ' ' // pretty formatting
    return `(${this.label_}${this.children_.map((x) => '\n' + indentation + x.toString(indentation)).join('')})`
  } else {
    return `(${this.label_} ${this.children_[0].toString(indentation)})`
  }
}

// export SyntaxTree
module.exports.SyntaxTree = SyntaxTree