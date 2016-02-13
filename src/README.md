
# High-level protocol overview

Each component has a protocol, this protocol is fairly abstract in that it's supposed to be easy to implement in various environment.

## Token

A token is just a container for some data, properties.

  - leading trivia: any leading white-space preceeding the token
  - type: the token type
  - text: the token text
  - lineNum: the line at which the token starts
  - charPos: the character position at which the token text starts (immideatly after any leading trivia)

## Lexer

The lexer has the following operations

  - load: decode a Unicode code point from the input buffer
  - read: consume a Unicode code point from the input buffer
  - next: tokenize input
  - yield: yield control back to the caller of the next function with a token value
  - error: retrive any error information (if an error is present)

# Coding conventions

## Member variables

Member variables are suffixed with an underscore `_` 

This is also used to declare intent, you should consider these to be **private**.

