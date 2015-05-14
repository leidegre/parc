The design goals of parc are:

- Simple
- Unambiguous
- Fast

parc is built specifically for deterministic context-free grammars. In other words, parc must be able to determine the next state transition from the top symbol.

> **Note:** It's my own general belief that ambiguous languages are of little practical use. The goal for parc is to be able to process any mainstream programming language but will initially focus on deterministic context-free grammars, DFA construction and pushdown automatons with fast O(n) run-time performance (to the extent possible).

parc is an execution environment and as such its front-/backend is decoupled from each other. The parc frontend processes parc grammar and builds a deterministic finite automaton and pushdown automaton representation that can be used to perform lexical analysis and textual processing. The parc backend takes these concepts and processes text. Constructing a syntax tree.

The result is a message pack (see http://msgpack.org/) object graph that can be used to add more deep semantic to text (such as syntax highlighting).

tree and it's why parc has support for projections (similar to [the now defunct project] Microsoft Oslo's' M language had). The projections in the parc grammar control the syntax tree construction and the labes that the syntax tree is assigned.

(concept borrowed from the Microsoft Roslyn Compiler Infrastructure) the original input can always be reconstructed from the syntax tree (even if it is ill-formed).

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

    token1 = "A" "B" "C" ;
    token2 = "A" - "C" ;
    token3 = "ABC" ;

Token (or production) rules cannot start with an optional repetition.

    token1 = "A" * ; -- illegal
    token2 = "A" ? ; -- illegal
    token3 = "A" + ; -- OK

parc will reject any token rule that results in an ambiguous (or non-deterministic) lexer. It will tell you exactly what the issue is what what you need to do to disambiguate the language rules.

There's also an unary complement operator that can be used to match anything which does not fall within a specific range.

    token1 = ^ "A" - "B" ;

This is useful to match a typical C-style string with any number of optional escape sequences in it.

    c_string = "\"" (^("\\" | "\"")* ("\\" ("\\" | "\""))?)* "\"" ;

The above token will match a quotation mark (") followed by anything which isn't a quotation mark or escape character (\). If it encounters either it will stop and accept an escape sequence and optionally repeat.

The above parc token rule is identical to the below regular expression.

    "([^\\"]*(\\("|\\))?)*"

We can break down this regular expression into parc syntax in the following way.

    "        -> "\""
    (        -> (
     [^\\"]* -> ^ ( "\\" | "\"" ) *
     (\\     -> ( "\\"
      ("|\\) -> ( "\"" | "\\" )
     )?      -> ) ?
    )*       -> ) *
    "        -> "\""

The difference is mostly in the way parc expects all characters to be encoded as strings. Regular expression character classes are simply choices in parc. `[0a!]` -> `"0" | "a" | "!"`. Note that a regular express does not allow more than one character per character class nor does it support nested character classes. parc does as long as the rules of determinism are followed.

### Graph builder algorithm

parc's lexer construction algorithm builds a decision tree based on token rules.

## Parsing (recursive descent parsing using pushdown automaton)
