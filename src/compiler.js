'use strict'

var Immutable = require('immutable')
var assert = require('./assert')

const CFGraph = require('./control-flow-graph')
  , Guard = require('./guard')

function Compiler() {
  this.graph_ = new CFGraph()
  this.root_ = this.graph_.newNode()
  // a quick note about that stack, each visiting function can expect that stack to be non-empty
  // but upon returning it must ensure that a top of the stack has been replaced with the next
  // node if any, it should not grow the stack unless there is recursion  
  this.stack_ = Immutable.Stack()
  this.negation_ = false
}

Compiler.prototype.Token = function(node) {
  const symbol = node.children_[0]
  // todo: define last as new symbol
  this.stack_ = this.stack_.push(this.root_)
  for (var i = 2, end = node.children_.length - 1; i < end; i++) {
    node.children_[i].accept(this)
    const next = this.stack_.peek()
    this.stack_ = this.stack_
      .pop()
      .pop()
      .push(next)
  }
  const last = this.stack_.peek()
  this.stack_ = this.stack_.pop() // pop last
  last.is_accept_state_ = true
}

// literal will grow the stack size
Compiler.prototype.Literal = function(node) {
  const prev = this.stack_.peek()
  this.stack_ = this.stack_.pop() // discard prev
  
  // traverse prev
  
  var guard = Guard.fromLiteral(this.negation_, node.children_[0])
  
  const edgeList = this.graph_.getEdgeListBySource(prev)
  const overlappingEdge = edgeList.find((edge) => edge.guard_set_.find((g) => g.overlaps(guard)))
  
  if (overlappingEdge) {
    const overlappingGuard = overlappingEdge.guard_set_.find((g) => g.overlaps(guard))
    if (overlappingGuard.equals(guard)) {
      // we can share this edge
      this.stack_ = this.stack_.push(overlappingEdge.target_)  
    } else {
      // todo: we need to split this edge
      throw new Error('Not implemented.')
    }
  } else {
    const next = this.graph_.newNode()
    const edge = this.graph_.newEdge(prev, next)
    edge.addGuard(guard)
    this.stack_ = this.stack_.push(next)  
  }
}

Compiler.prototype.Negation = function(node) {
  this.negation_ = !this.negation_
  node.children_[1].accept(this)
  this.negation_ = !this.negation_
}

Compiler.prototype.Choice = function (node) {
  // a boolean operator
  const prev = this.stack_.peek()
  this.stack_ = this.stack_.pop() // pop prev
  
  const enter = this.graph_.newNode()
  
  this.stack_ = this.stack_.push(enter)
  node.getChildAt(0).accept(this)
  
  const left = this.stack_.peek()
  this.stack_ = this.stack_.pop() // pop left
  
  this.stack_ = this.stack_.push(enter)
  node.getChildAt(2).accept(this)
  
  const right = this.stack_.peek()
  this.stack_ = this.stack_.pop() // pop right
  
  const leave = this.graph_.newNode()
  this.stack_ = this.stack_.push(leave)
  
  this.graph_.newEdge(prev, enter)
  this.graph_.newEdge(left, leave)
  this.graph_.newEdge(right, leave)
}

Compiler.prototype.Quantifier = function(node) {
  // Apply the Kleene star to this syntax node
  kleeneStar.call(this, node.children_[0])
}

// The Kleene star is simply a construction that allows for any number of repetitions of a syntax rule
// Since we're trying to build a NFA we can use dummy nodes (e-closure) for enter/leave states.
// This all works out very well.
function kleeneStar(node, kleenePlus) {
  
  // applying the Kleene star like this easy but I think it could be wrong
  // the dummy enter node is hiding the fact that we
  // have a shared edge later that we may transition through
  // though, this may be okay becuase we we compute the DFA
  // we might actually eleminate these, tbd, I should probably
  // investigate this further
  
  const enter = this.graph_.newNode()
  const leave = this.graph_.newNode()
  const begin = this.stack_.peek()
  this.stack_ = this.stack_.pop() // discard begin
  this.stack_ = this.stack_.push(enter)
  node.accept(this)
  const end = this.stack_.peek()
  this.stack_ = this.stack_.pop() // discard end
  this.graph_.newEdge(begin, enter)
  this.graph_.newEdge(end, leave)
  if (!kleenePlus) this.graph_.newEdge(enter, leave) // The Kleene star is optional but the Kleene plus is not
  this.graph_.newEdge(leave, enter).is_backlink_ = true // repeated
  this.stack_ = this.stack_.push(leave)
}

module.exports = Compiler