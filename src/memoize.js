'use strict'

function memoize(fn) {
  var prevArg
  var prevResult
  return (arg) => {
    if (arg !== prevArg) {
      prevArg = arg
      prevResult = fn.call(this, arg)
    }
    return prevResult
  }
}

module.exports = memoize