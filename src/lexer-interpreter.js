'use strict'

// The lexer interpreter can run DFAs that do lexical analysis. It's basically a tokenizer that you can plug DFAs into.

class Reader {
  constructor(inp) {
    this.value_ = 0
    this.inp_ = inp
    this.index_ = 0
    this.length_ = inp.length
  }
  load() {
    if (this.index_ < this.length_) {
      this.value_ = this.inp_.charCodeAt(this.index_)
      return true
    }
    return false
  }
  read() {
    if (this.index_ < this.length_) {
      this.index_++
    } else {
      throw new Error('Cannot read past end of stream.')
    }
  }
}

class Tokenizer {
  tokenize() {

  }
}



module.exports = Tokenizer