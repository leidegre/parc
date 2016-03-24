'use strict'

function Interval(a, b) {
  if (typeof a !== 'number') throw Error('a must be number')
  if (typeof b !== 'number') throw Error('b must be number')
  if (!(a <= b)) throw Error('Interval: !(a <= b)')
  this.a = a
  this.b = b
  Object.freeze(this)
}

Interval.prototype.equals = function (other) {
  if (other.constructor !== Interval) throw Error('other must be Interval')
  const x = other.a
  const y = other.b
  return this.a === x && this.b === y
}

Interval.prototype.overlaps = function (other) {
  if (other.constructor !== Interval) throw Error('other must be Interval')
  const x = other.a
  const y = other.b
  return this.a <= y && x <= this.b
}

Interval.prototype.contains = function (other) {
  if (typeof other !== 'number') throw Error('other must be number')
  return this.a <= other && other <= this.b
}

Interval.prototype.complement = function (other) {
  if (other.constructor !== Interval) throw Error('other must be Interval')
  if (!this.overlaps(other)) {
    return [this] // no change
  }
  const x = other.a
  const y = other.b
  if (x <= this.a && y < this.b) {
    return [new Interval(y + 1, this.b)]
  }
  if (this.a < x && this.b <= y) {
    return [new Interval(this.a, x - 1)]
  }
  if (this.a < x && y < this.b) { // cut-out
    return [new Interval(this.a, x - 1), new Interval(y + 1, this.b)]
  }
  // the intervals are either equal or other extends over this
  return []
}

Interval.prototype.toJSON = function () {
  return [this.a, this.b]
}

Interval.prototype.toString = function () {
  return `[${this.a},${this.b}]`
}

module.exports = Interval