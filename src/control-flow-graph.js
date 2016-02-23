'use strict'

var Immutable = require('immutable')

function GraphNode(id) {
  if (typeof id !== 'number') throw new TypeError('id must be number')
  this.id_ = id
  this.edge_weight_ = 0 // num edges in or out
  this.is_accept_state_ = false
}

function GraphEdge(source, target) {
  if (source.constructor !== GraphNode) throw TypeError('source must be CFGraphNode')
  if (target.constructor !== GraphNode) throw TypeError('target must be CFGraphNode')
  this.source_ = source
  this.target_ = target
  // it's a rather complex to figure out cycles in a graph but 
  // since we know when we add edges that create cycles we can flag them like so
  this.is_backlink_ = false
  this.guard_set_ = Immutable.List()
}

GraphEdge.prototype.addGuard = function (guard) {
  this.guard_set_ = this.guard_set_.push(guard)
}

function Graph() {
  this.node_id_ = 0
  this.nodes_ = []
  this.edges_by_source_ = Immutable.Map()
  this.edges_by_target_ = Immutable.Map()
}

Graph.prototype.newNode = function() {
  var node = new GraphNode(this.node_id_++)
  this.nodes_.push(node)
  return node
}

Graph.prototype.newEdge = function(source, target) {
  var edge = new GraphEdge(source, target)
  source.edge_weight_++
  target.edge_weight_++
  this.edges_by_source_ = addEdgeToIndex(source, edge, this.edges_by_source_)
  this.edges_by_target_ = addEdgeToIndex(target, edge, this.edges_by_target_)
  return edge
}

Graph.prototype.getEdgeListBySource = function(source) {
  const edgeList = this.edges_by_source_.get(source)
  return edgeList ? edgeList : Immutable.List()  
}

Graph.prototype.getEdgeListByTarget = function(target) {
  const edgeList = this.edges_by_target_.get(target)
  return edgeList ? edgeList : Immutable.List()  
}

function edgeComparator(x, y) {
  // by source id, then by target id
  return x.source_.id_ != y.source_.id_ 
    ? x.source_.id_ - y.source_.id_ 
    : x.target_.id_ - y.target_.id_  
}

function edgeMap(edge) {
  return {
    source: edge.source_.id_, 
    target: edge.target_.id_, 
    guard_set: edge.guard_set_ ? edge.guard_set_ : undefined,
    is_backlink: edge.is_backlink_
  }
}

Graph.prototype.toJSON = function () {
  // doesn't matter if we go by source or target but are complete edge lists
  const edges = this.edges_by_source_
    .valueSeq()
    .flatten()
    .sort(edgeComparator)
  return {
    nodes: this.nodes_.map((node) => ({ id: node.id_, is_accept_state: node.is_accept_state_ })),
    edges: edges.map(edgeMap).toArray(),
  }
}

function addEdgeToIndex(node, edge, index) {
  var edgeList = index.get(node)
  if (edgeList) {
    return index.set(node, edgeList.push(edge))
  } else {
    return index.set(node, Immutable.List.of(edge))
  }
}

module.exports = Graph