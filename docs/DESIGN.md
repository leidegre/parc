The design goals of parc are:

- Simple
- Disambiguous
- Fast

parc has been built specifically for LL(1) context-free grammar. In other words, the lexer (and parser) that parc generates run in O(n) but require that the lexer (or parser) can transition to it's next state given the next character (or token) of the input stream.

> **Note:** It's my own general belief that ambiguous languages are of little practical use. Stretch goals for parc is of course to be able to handle any mainstream programming language but we will initially limit ourselves to these LL(1) context-free grammars and focus on DFA construction and fast run-time performance.

The parc backend is decoupled from it's frontend. By this we mean that the main parc program can interpret the grammar and execute any production for some text input. The result is a message pack (see http://msgpack.org/) dynamic syntax tree and it's why parc has support for projections (similar to [the now defunct project] Microsoft Oslo's' M language had). The projections in the parc grammar control the syntax tree construction and the labes that the syntax tree is assigned.

The parc frontend accept this dynamic syntax tree which is a complete rendition of the input. Through the use of syntax trivia (concept borrowed from the Microsoft Roslyn Compiler Infrastructure) the original input can always be reconstructed from the syntax tree (even if it is ill-formed).

## Tokenization (lexical analysis)

Tokens are defined by identifiers that start with a lower case letter (identifiers may not start with digits).

    token = "A" - "Z" ;

The above token matches a single upper case letter between A and Z (inclusive).

    token = "A" - "Z" ("A" - "Z") * ;

The above token matches at least one occurrence but may match more. And is equivalent to the following notation.

    token = ("A" - "Z") + ;

Parenthesis are not required but recommended for clarity.

We can also introduce alterations (or choice).

    token = "A" | "B" | "C" ;

The above simple rules form the basis for a regular language construct that is used to describe how to match a token.

Note that the following are all examples of the same, identical token (these are ambiguous and illegal).

    token = "A" "B" "C" ;
    token = "A" - "C" ;
    token = "ABC" ;

Token (or production) rules cannot start with an optional repetition.

    token = "A" * ; -- illegal
    token = "A" ? ; -- illegal
    token = "A" + ; -- OK

parc will reject any token rules that results in an ambiguous (or non-deterministic) lexer. It will tell you exactly what the issue is what what you need to do to disambiguate the language rules.

Thompson's Construction Algorithm
==

Three basic rules. Empty expression, symbol and union (NFA).

We then express our extensions in the form of these rules.

"A" - "C" -> "A" | "B" | "C"
"A" +     -> "A" "A" *
"A" ?     -> "A" | Empty

Complement sucks ass.

Alphabet construction. Even if we define a character range that span the entirety of the Unicode code space we only need to define an alphabet member for each situation in which the character is used in a unique state transition. Hence a negative character class `^ "\""` map the `\"` character to an exit state and everything else to a accept state.
