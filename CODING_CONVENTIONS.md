
# Constraints

Why do we need constraints? See Chet Faliszek talk: How to get yourself a job in the gaming industry.

# var, let and const?

Use `var`, declare varibles once in the begining of the function. 

No exceptions.

> **note:** that varibles declared with `var` are scoped to the enclosing function.

## Why?

`let` and `const` also knowns as, extraneous and superfluous. While `let` and `const` express intent, this is intent can just as well be expressed by functions.

This constraint requires that we think about the design of our functions to greater extent, which is the main point of these coding guidelines. Design first.

# 'use strict' && ES6 classes

There's nothing wrong with ES6 classes.

~~~
class Sandwich extends Edible {
  constructor() {
    super()
  }
  eat() {
    //...
  }
}
~~~

...is just a shorter notation for...

~~~
function Sandwich() {
  Edible.call(this)
}
Sandwich.prototype = Object.create(Edible.prototype)
Sandwich.prototype.constructor = Sandwich
Sandwich.prototype.eat = function eat() {
  //...
}
~~~

...and it's a straightforward way for us to group a set of related functions.

 