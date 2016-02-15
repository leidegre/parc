'use strict'

var Immutable = require('immutable')

function GraphNode(id) {
  if (typeof id !== 'number') throw new TypeError('id must be number')
  this.id_ = id
}

function GraphEdge(source, target) {
  if (source.constructor !== GraphNode) throw TypeError('source must be GraphNode')
  if (target.constructor !== GraphNode) throw TypeError('target must be GraphNode')
  this.source_ = source
  this.target_ = target
  // it's a rather complex to figure out cycles in a graph but 
  // since we know when we add edges that create cycles we can flag them like so
  this.is_backlink_ = false
}

function Graph() {
  this.node_id_ = 1
  this.nodes_ = []
  this.edges_by_source_ = Immutable.Map()
  this.edges_by_target_ = Immutable.Map()
}

Graph.prototype.newNode = function() {
  var node = new GraphNode(this.node_id_++)
  this.nodes_.push(node)
  return node
}

function addEdgeToIndex(node, edge, index) {
  var edgeList = index.get(node)
  if (edgeList) {
    edgeList.push(edge)
    return index // no change
  } else {
    return edgeList.set(node, [edge])
  }
}

Graph.prototype.newEdge = function(source, target) {
  var edge = new GraphEdge(source, target)
  this.edges_by_source_ = addEdgeToIndex(source, edge, this.edges_by_source_)
  this.edges_by_target_ = addEdgeToIndex(target, edge, this.edges_by_target_)
  return edge
}

