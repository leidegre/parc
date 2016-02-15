
var test = require('tape')

var Particle = require('./')

test('parse empty namespace', function(t) {
  t.plan(1)
  var syntax = Particle.parse('"parc" { }')
  t.equal(syntax.toString(), '(Grammar (Namespace "parc" { }))')
  t.end()
})

test('parse token rule 1', function(t) {
  t.plan(1)
  var syntax = Particle.parse('"parc" { local = "A" ; }')
  t.equal(syntax.toString(), '(Grammar (Namespace "parc" { (Token local = (Literal "A") ;) }))')
  t.end()
})

test('parse token rule 2', function(t) {
  t.plan(1)
  var syntax = Particle.parse('"parc" { local = "A" .. "Z" ; }')
  t.equal(syntax.toString(), '(Grammar (Namespace "parc" { (Token local = (Range (Literal "A") .. (Literal "Z")) ;) }))')
  t.end()
})

test('parse token rule 3', function(t) {
  t.plan(1)
  var syntax = Particle.parse('"parc" { local = "\\"" ^ "\\"" * "\\"" ; }')
  t.equal(syntax.toString(), '(Grammar (Namespace "parc" { (Token local = (Literal "\\"") (Quantifier (Negation ^ (Literal "\\"")) *) (Literal "\\"") ;) }))')
  t.end()
})