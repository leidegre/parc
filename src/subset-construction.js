'use strict'

// http://www.cs.nuim.ie/~jpower/Courses/Previous/parsing/node9.html

const Immutable = require('immutable')
const memoize = require('./memoize')

function computeId(state) {
  return state.map((node) => node.id_).sort().toArray().join(',')
}

// The DFA state itself is defined by a set of NFA states
function State() {
  this.state_ = Immutable.Set()
  // hmm...
  var memoizedComputeId = memoize(computeId)
  Object.defineProperty(this, 'id', {
    get: function() { return memoizedComputeId(this.state_) }
  })
  this.transitions_ = Immutable.List()
}

// add NFA state
State.prototype.add = function(node) {
  const newState = this.state_.add(node)
  if (newState !== this.state_) {
    this.state_ = newState
    return true
  }
  return false
}

// add DFA transition
State.prototype.addTransition = function(guard, nextState) {
  this.transitions_ = this.transitions_.push([guard, nextState])
}

function SubsetConstruction(graph) {
  this.g_ = graph
}

SubsetConstruction.prototype.construct = function(root) {
  var initialState = new State()
  closure(this.g_, root, initialState)
  construct(this.g_, initialState, Immutable.Map())
  return initialState
}

// computes the ε-closure
function closure(graph, node, state) {
  if (state.add(node)) {
    for (let edge of graph.getEdgeListBySource(node).filter((edge) => !edge.hasGuard)) {
      closure(graph, edge.target_, state)
    }
  }
}

function move(graph, state, symbol, newState) {
  for (let v of state.state_) {
    for (let e of graph.getEdgeListBySource(v)) {
      if (e.guard_set_.find((guard) => guard.overlaps(symbol))) {
        closure(graph, e.target_, newState)
      }
    }
  }
}

function construct(graph, currState, visited) {
  const input = currState.state_
    .map((node) => graph.getEdgeListBySource(node))
    .flatten()
    .map((edge) => edge.guard_set_)
    .flatten()
  for (let x of input) {
    const newState = new State()
    move(graph, currState, x, newState)
    const prevState = visited.get(newState.id)
    if (prevState) {
      currState.addTransition(x, prevState)
    } else {
      currState.addTransition(x, newState)
      // recursivly construct the DFA
      visited = visited.set(newState.id, newState)
      visited = construct(graph, newState, visited)
    }
  }
  return visited
}

module.exports = SubsetConstruction