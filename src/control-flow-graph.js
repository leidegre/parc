'use strict'

var Immutable = require('immutable')

function CFGraphNode(id) {
  if (typeof id !== 'number') throw new TypeError('id must be number')
  this.id_ = id
}

function CFGraphEdge(source, target) {
  if (source.constructor !== CFGraphNode) throw TypeError('source must be CFGraphNode')
  if (target.constructor !== CFGraphNode) throw TypeError('target must be CFGraphNode')
  this.source_ = source
  this.target_ = target
  // it's a rather complex to figure out cycles in a graph but 
  // since we know when we add edges that create cycles we can flag them like so
  this.is_backlink_ = false
  this.guard_set_ = null
}

CFGraphEdge.prototype.addGuard = function (guard) {
  // todo: may need to split?
  if (!this.guard_set_) {
    this.guard_set_ = []
  }
  this.guard_set_.push(guard)
}

function CFGraph() {
  this.node_id_ = 1
  this.nodes_ = []
  this.edges_by_source_ = Immutable.Map()
  this.edges_by_target_ = Immutable.Map()
}

CFGraph.prototype.newNode = function() {
  var node = new CFGraphNode(this.node_id_++)
  this.nodes_.push(node)
  return node
}

CFGraph.prototype.newEdge = function(source, target) {
  var edge = new CFGraphEdge(source, target)
  this.edges_by_source_ = addEdgeToIndex(source, edge, this.edges_by_source_)
  this.edges_by_target_ = addEdgeToIndex(target, edge, this.edges_by_target_)
  return edge
}

CFGraph.prototype.toJSON = function () {
  // doesn't matter if we go by source or target but are complete edge lists
  const edges = this.edges_by_source_
  return {
    nodes: this.nodes_.map((node) => ({ id: node.id_ })),
    edges: edges.map((v) => Immutable.List(v)).flatten().map((e) => ({ from: e.source_.id_, to: e.target_.id_, guard_set: e.guard_set_ ? e.guard_set_ : undefined })).toArray(),
  }
}

function addEdgeToIndex(node, edge, index) {
  var edgeList = index.get(node)
  if (edgeList) {
    edgeList.push(edge)
    return index // no change
  } else {
    return index.set(node, [edge])
  }
}

module.exports = CFGraph