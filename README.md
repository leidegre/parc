# particle (or parc for short) is a parser compiler environment

It's written in JavaScript and can be run in a Node.js environment or modern browser. 

An overarching goal of this project is to provide a clean and easy to understand code base.

 A nifty thing about particle is that it compiles your grammar to byte code. This byte code can be run in any VM that implements the VM protocol (which is small). Another thing you can do is that you can take the byte code and emit LLVM byte code that can be compiled into a binary. This way we can actually have a really fast compiler frontend.
 
 # Test watch
 
 Use `nodemon` to run tests automatically as you're working on the code.
 
 ~~~
 nodemon --exec "node src\compiler-tests.js | faucet" --watch src\compiler.js --watch src\compiler-tests.js
 ~~~