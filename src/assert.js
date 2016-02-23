'use strict'

class AssertionError extends Error {
  constructor(message) {
    super(message)
  }
}

function Assert(value, message) {
  if (!value) {
    throw new AssertionError(message || 'Assertion failed!')
  }
}

module.exports = Assert