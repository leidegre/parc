'use strict'

var test = require('tape')

var Particle = require('./')
  , SyntaxVisitor = require('./syntax-visitor')

test('syntax visitor', function(t) {
  var syntax = Particle.parse('"parc" { local = "\\"" ^ "\\"" * "\\"" ; }')
  var visitor = new SyntaxVisitor()
  visitor.__accept__(syntax)
  
  console.log(JSON.stringify(visitor.graph_, null, 2))
  
  t.end()
})