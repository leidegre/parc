
A note about coding style
==

Error handling
===

In general a function should return a value, this value should be an `ìnt` the value of this `ìnt` should be non-zero for success and zero for failure.

This is done to support a consistent control-flow for error handling.

    if (!fun(...)) {
      // further error handling goes here...
      return 0;
    }

If a function has more error information to share. That error information should be stored in a mutable state that gets passed to the function.

Motivation
====

For the most part we only need to know whether the function succeeded or not. Functions that support additional error information should pass around some mutable state that can store this error information. This way can signal error and unwind accordingly when needed without losing error information.

Order of arguments
===

As a general rule, the arguments to the left are inputs and the arguments to the right are outputs. This is shown by the general tendency to pass input arguments as read-only, `const` qualified (immutable) and output arguments as pointer to some memory location that is writable.

* Exceptions to this rule: Member functions

Member functions
===

While there is no such thing as a member function in C a struct `some_struct` with the member function `add` is a function `some_struct_add` that takes as it's first argument a pointer to `some_struct`. This convention is used to group a set of functions that mutate some shared state.

Function overloading
===

C does not support function overloading. Instead we incorporate a naming convention into the function name to support common types. For example:

- `char` -> `_char`
- `int` -> `_int`
- `unsigned int` -> `_uint`
- `void*` -> `_ptr`
- `const char *`, `size_t` -> `_str`; *note: length-delimited*

Note that we do not pass strings with the expectation that they are null terminated. While this might be true we always expect that the string size (bytes **NOT** characters) is passed alongside string arguments, always.
