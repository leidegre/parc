'use strict'

const test = require('tape')
const Interval = require('./interval')

test('Interval constructor', (t) => {
  t.plan(4)

  var x = new Interval(1, 2)
  t.equal(x.a, 1)
  t.equal(x.b, 2)

  var y = new Interval(3, 4)
  t.equal(y.a, 3)
  t.equal(y.b, 4)
})

test('Equal intervals', (t) => {
  t.plan(2)

  var x = new Interval(1, 2)
  t.ok(x.equals(new Interval(1, 2)))

  var y = new Interval(3, 4)
  t.ok(y.equals(new Interval(3, 4)))
})

test('Not equal intervals', (t) => {
  t.plan(2)

  var x = new Interval(1, 2)
  t.notOk(x.equals(new Interval(2, 3)))

  var y = new Interval(3, 4)
  t.notOk(y.equals(new Interval(2, 3)))
})

test('Overlapping intervals', (t) => {
  t.plan(4)

  var x = new Interval(1, 2)
  t.ok(x.overlaps(new Interval(0, 1)))
  t.ok(x.overlaps(new Interval(1, 1)))
  t.ok(x.overlaps(new Interval(1, 2)))
  t.ok(x.overlaps(new Interval(2, 3)))
})

test('Non-overlapping intervals', (t) => {
  t.plan(4)

  var x = new Interval(1, 2)
  t.notOk(x.overlaps(new Interval(0, 0)))
  t.notOk(x.overlaps(new Interval(3, 3)))
  t.notOk(x.overlaps(new Interval(3, 4)))
  t.notOk(x.overlaps(new Interval(3, 5)))
})

test('Interval contains', (t) => {
  t.plan(2)

  var x = new Interval(1, 2)
  t.ok(x.contains(1))
  t.ok(x.contains(2))
})

test('Interval does not contain', (t) => {
  t.plan(2)

  var x = new Interval(1, 2)
  t.notOk(x.contains(0))
  t.notOk(x.contains(3))
})

test('Interval complement self result in empty array', (t) => {
  t.plan(3)

  var x = new Interval(1, 2)
  t.deepEqual(x.complement(x), [])

  var y = new Interval(0, 1)
  t.deepEqual(y.complement(y), [])

  var z = new Interval(2, 3)
  t.deepEqual(z.complement(z), [])
})

test('Interval complement of non-overlapping intervals result in same', (t) => {
  t.plan(2)

  var x = new Interval(1, 2)
  var z = new Interval(3, 4)

  t.deepEqual(x.complement(z), [x])
  t.deepEqual(z.complement(x), [z])
})

test('Interval complement of lower half', (t) => {
  t.plan(1)

  var x = new Interval(0, 5)
  var y = new Interval(0, 1)
  var z = new Interval(2, 5)

  t.deepEqual(x.complement(y), [z])
})

test('Interval complement of upper half', (t) => {
  t.plan(1)

  var x = new Interval(0, 5)
  var y = new Interval(4, 5)
  var z = new Interval(0, 3)

  t.deepEqual(x.complement(y), [z])
})

test('Interval cut-out', (t) => {
  t.plan(1)

  var x = new Interval(0, 5)
  var y = new Interval(2, 3)
  var z = new Interval(0, 1)
  var w = new Interval(4, 5)

  t.deepEqual(x.complement(y), [z, w])
})

function nextInterval() {
  const x = parseInt(11 * Math.random())
  let y = parseInt(11 * Math.random())
  while (!(x <= y)) {
    y = parseInt(11 * Math.random())
  }
  return new Interval(x, y)
}

// Convert an Interval or array of Intervals to an array of finite elements.
function toRange(source) {
  source = Array.isArray(source) ? source : [source]
  var arr = []
  for (let i = 0; i < source.length; i++) {
    const interval = source[i]
    if (interval.constructor !== Interval) throw Error('expected Interval')
    for (let j = interval.a; j <= interval.b; j++) {
      arr.push(j)
    }
  }
  return arr
}

function complement() {
  let a = []
  let b = arguments[0]
  for (let i = 0; i < b.length; i++) {
    let x = b[i]
    let j
    for (j = 1; j < arguments.length; j++) {
      let c = arguments[j]
      if (c.indexOf(x) !== -1) {
        break
      }
    }
    if (j === arguments.length) {
      a.push(x)
    }
  }
  return a
}

test('Interval to range', (t) => {
  t.plan(5)
  t.deepEqual(toRange(new Interval(0, 0)), [0])
  t.deepEqual(toRange(new Interval(0, 1)), [0, 1])
  t.deepEqual(toRange(new Interval(1, 2)), [1, 2])
  t.deepEqual(toRange(new Interval(2, 4)), [2, 3, 4])
  t.deepEqual(toRange(new Interval(1, 9)), [1, 2, 3, 4, 5, 6, 7, 8, 9])
})

test('Complement of range', (t) => {
  t.plan(3)
  t.deepEqual(complement([1, 2, 3], [2, 3]), [1])
  t.deepEqual(complement([1, 2, 3, 4], [1, 4]), [2, 3])
  t.deepEqual(complement([1, 2, 3, 4], [2], [3]), [1, 4])
})

test('Interval combinatorial tests', (t) => {
  for (let i = 0; i < 1000; i++) {
    const x = nextInterval()
    const y = nextInterval()
    
    // the following is a functional specification
    if (x.overlaps(y)) {
      t.ok(y.overlaps(x)) // overlap is associative
      
      const z = x.complement(y)
      
      t.deepEqual(toRange(z), complement(toRange(x), toRange(y)))
    }
  }
  t.end()
})