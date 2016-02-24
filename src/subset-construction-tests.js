'use strict'

const test = require('tape')
const Immutable = require('immutable')

var particle = require('./')

function show(dfa, set) {
  if (set.has(dfa.id)) {
    return set
  }
  set = set.add(dfa.id) // no "loops"
  console.log(`state [${dfa.id}]`)
  for (let x of dfa.transitions_) {
    console.log(` -> ${x[0]} [${x[1].id}]`)
  }
  for (let x of dfa.transitions_) {
    set = show(x[1], set)
  }
  return set
}

test('DFA[a(a|b)]', function(t) {
  var dfa = particle.construct('"parc" { a = "A" ( "A" | "B" ) ; }')
  show(dfa.dfa, new Immutable.Set())
  t.end()
})

test('DFA[a(a|b)c*]', function(t) {
  var dfa = particle.construct('"parc" { a = "A" ( "A" | "B" ) "C" * ; }')
  show(dfa.dfa, new Immutable.Set())
  t.end()
})

test('DFA[a(a|b)c+]', function(t) {
  var dfa = particle.construct('"parc" { a = "A" ( "A" | "B" ) "C" + ; }')
  show(dfa.dfa, new Immutable.Set())
  t.end()
})