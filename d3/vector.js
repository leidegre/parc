'use strict'

function Vector2D(x, y) {
  this.x = x
  this.y = y
}

// static
Vector2D.from = function (obj) {
  return new Vector2D(obj.x, obj.y)
}

Vector2D.lerp = function (a, b, t) {
  return new Vector2D((1.0 - t) * a.x + t * b.x, (1.0 - t) * a.y + t * b.y)
}

// instance
Vector2D.prototype.add = function (other) {
  return new Vector2D(this.x + other.x, this.y + other.y)
}

Vector2D.prototype.sub = function (other) {
  return new Vector2D(this.x - other.x, this.y - other.y)
}

Vector2D.prototype.length = function () {
  return Math.sqrt(this.x * this.x + this.y * this.y)
}

Vector2D.prototype.normalize = function () {
  const l = this.length()
  return new Vector2D(this.x / l, this.y / l)
}

Vector2D.prototype.scale = function (k) {
  return new Vector2D(k * this.x, k * this.y)
}

Vector2D.prototype.perp = function () {
  return new Vector2D(-this.y, this.x)
}

Vector2D.prototype.toJSON = function () {
  return { x: this.x, y: this.y }
}

Vector2D.prototype.toString = function () {
  return `${ this.x.toFixed(3) },${ this.y.toFixed(3) }`
}

module.exports = Vector2D
