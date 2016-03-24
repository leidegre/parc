'use strict'

var test = require('tape')
var particle = require('./')

var Interval = require('./interval')

// these are just theoretical edge cases they won't necessarily occur in practice
// however, it's not that hard cover them precisely and get a correct compiler that way

//  "A" 
//  "A" - "B"
//  "A" | "B"
// ("A" - "C") \ "B"

// 0 - [], [] -> 1

function show(g) {
  let sb = ''
  for (let item of g.edges_by_source_) {
    let edge_list = item[1]
    for (let edge of edge_list) {
      sb += `# ${edge.source_.id_} - ${edge.guard_set_.toArray().join(', ')} -> ${edge.target_.id_} ${edge.target_.is_accept_state_ ? 'A' : ' '}\n`
    }
  }
  return sb
}

test('compile lexer', (t) => {
  t.plan(1)
  
  let compilation = particle.compile('"parc"{a="A";}')
  
  console.log(show(compilation.graph))
  
  let edge_list = compilation.graph.getEdgeListBySource(compilation.root)
  let edge = edge_list.get(0)
  let guard = edge.guard_set_.get(0)
  
  t.deepEqual(guard, new Interval(0x41, 0x41))
})

test('compile lexer', (t) => {
  t.plan(1)
  
  let compilation = particle.compile('"parc"{a="A".."B";}')
  
  console.log(show(compilation.graph))
  
  let edge_list = compilation.graph.getEdgeListBySource(compilation.root)
  let edge = edge_list.get(0)
  let guard = edge.guard_set_.get(0)
  
  t.deepEqual(guard, new Interval(0x41, 0x42))
})

test('compile lexer', (t) => {
  t.plan(1)
  
  let compilation = particle.compile('"parc"{a="A".."C";}')
  
  console.log(show(compilation.graph))
  
  let edge_list = compilation.graph.getEdgeListBySource(compilation.root)
  let edge = edge_list.get(0)
  let guard = edge.guard_set_.get(0)
  
  t.deepEqual(guard, new Interval(0x41, 0x43))
})

test('compile lexer', (t) => {
  t.plan(2)
  
  let compilation = particle.compile('"parc"{a="A";b="B";}')
  
  console.log(show(compilation.graph))
  
  let edge_list = compilation.graph.getEdgeListBySource(compilation.root)
  
  let edge = edge_list.get(0)
  let guard = edge.guard_set_.get(0)
  
  t.deepEqual(guard, new Interval(0x41, 0x41))
  
  let edge1 = edge_list.get(1)
  let guard1 = edge1.guard_set_.get(0)
  
  t.deepEqual(guard1, new Interval(0x42, 0x42))
})


test('compile lexer', (t) => {
  // t.plan(-1)
  
  let compilation = particle.compile('"parc"{a="A" "A";b="A" "a";}')
  
  console.log(show(compilation.graph))
  
  t.end()
})