
# High-level protocol overview

Each component has a protocol, this protocol is fairly abstract in that it's supposed to be easy to implement in various environments.

## Token

A token is just a container for some data, properties.

  - `leading_trivia_`: any leading white-space preceeding the token
  - `type_`: the token type
  - `s_`: a slice of the input buffer
  - `line_num_`: the line at which the token starts
  - `char_pos_`: the character position at which the token text starts (immideatly after any leading trivia)

## Lexer

The lexer has the following operations

  - `load`: decode a Unicode code point (low-level)
  - `read`: consume a Unicode code point (low-level)
  - `next`: tokenize
  - `yield`: yield control back to the caller (low-level)
  - `token_`: the last token that was successfully tokenized
  - `err_`: the last error if token was not successfully tokenized

*\* = low-level APIs are only needed if you intend to hack on your own compiler infrastructure.*

# Coding conventions

## Naming conventions

When choosing a name, prefer long names to abbreviated names. Prefer descriptive names to arbitrary ones.

> **NOTE:** In no case should any name or label (for a variable, class, namespace, file, and so on) run words together without any kind of delineation (such as an underscore or a change in case according to the standards set herein).

### Local variables (incl. parameters)

- No leading underscores.
- No trailing underscores.
- `lower_case_underscores`

> **Exception:** in a temporary small scope single letter (or really short) identifiers are OK.

### Member variables (aka data members)

- No leading underscores.
- Single trailing underscore.
- `lower_case_single_underscore_at_end_`

### Functions

- PascalCase
- No underscores.
