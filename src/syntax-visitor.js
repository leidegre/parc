'use strict'

var syntax = require('./syntax')
  , SyntaxTree = syntax.SyntaxTree

function SyntaxVisitor() {
}

/**
 * @param {SyntaxTree} node
 */
SyntaxVisitor.prototype.__visit__ = function(node) {
  if (node.constructor !== SyntaxTree) throw new TypeError('node must be SyntaxTree')
  const visitor = this[node.label_] // lookup visit function based on label
  if (visitor) {
    visitor(node)
  } else {
    this.__accept__(node)
  }
}

/**
 * @param {SyntaxTree} node
 */
SyntaxVisitor.prototype.__accept__ = function(node) {
  if (node.constructor !== SyntaxTree) throw new TypeError('node must be SyntaxTree')
  const children = node.children_
  for (let i = 0, end = children.length; i < end; i++) {
    const childNode = children[i]
    if (node.constructor === SyntaxTree) {
      this.__visit__(node)
    }
  }  
}

SyntaxVisitor.prototype.Grammar = function(node) {
  for (var i = 0; i < node.children_.length; i++) {
    this.__accept__(node.children_[i])
  }
}

SyntaxVisitor.prototype.Namespace = function(node) {
  for (var i = 2, end = node.children_.length - 1; i < end; i++) {
    this.__accept__(node.children_[i])
  }
}

SyntaxVisitor.prototype.Token = function(node) {
  const symbol = node.children_[0]
  var root = this.graph_.newNode()
  // todo: define symbol
  this.stack_ = this.stack_.push(root)
  for (var i = 2, end = node.children_.length - 1; i < end; i++) {
    this.__accept__(node.children_[i])
    const next = this.stack_.peek()
    this.stack_ = this.stack_
      .pop()
      .pop()
      .push(next)
  }
}

// literal will grow the stack size
SyntaxVisitor.prototype.Literal = function(node) {
  const prev = this.stack_.peek()
  this.stack_ = this.stack_.pop() // discard prev
  const next = this.graph_.newNode() // note: we might discard this node
  const edge = this.graph_.newEdge(prev, next)
  
  // todo:  there might already be a way to get here, we need to check for that before we add another edge
  
  var guard = Guard.fromLiteral(this.negation_, node.children_[0])
  
  edge.addGuard(guard)
  
  this.stack_ = this.stack_.push(next)
}

SyntaxVisitor.prototype.Negation = function(node) {
  this.negation_ = !this.negation_
  this.__accept__(node.children_[1])
  this.negation_ = !this.negation_
}

SyntaxVisitor.prototype.Quantifier = function(node) {
  // Apply the Kleene star to this syntax node
  kleeneStar.call(this, node.children_[0])
}

// The Kleene star is simply a construction that allows for any number of repetitions of a syntax rule
// Since we're trying to build a NFA we can use dummy nodes (e-closure) for enter/leave states.
// This all works out very well.
function kleeneStar(node, kleenePlus) {
  const enter = this.graph_.newNode()
  const leave = this.graph_.newNode()
  const begin = this.stack_.peek()
  this.stack_ = this.stack_.pop() // discard begin
  this.stack_ = this.stack_.push(enter)
  this.__accept__(node)
  const end = this.stack_.peek()
  this.stack_ = this.stack_.pop() // discard end
  this.graph_.newEdge(begin, enter)
  this.graph_.newEdge(end, leave)
  if (!kleenePlus) this.graph_.newEdge(enter, leave) // The Kleene star is optional but the Kleene plus is not
  this.graph_.newEdge(leave, enter).is_backlink_ = true // repeated
  this.stack_ = this.stack_.push(leave)
}

module.exports = SyntaxVisitor