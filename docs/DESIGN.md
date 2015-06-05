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

## Parsing (recursive descent parsing with a stack machine)

To understand recursive decent parsing we'll be looking at a reference source implementation and then translate that into a intermediate representation (byte code) that we can run in a stack machine.

Let's define a grammar. We'll be using a small grammar for that can be used to represent expressions (with explicit order of evaluation, parentheses).

    --tokens
    number = ("0" - "9") + ;
    operator = "+" | "-" | "*" | "/" ;

    --productions
    Expression = PrimaryExpression
      | PrimaryExpression operator Expression
      ;
    PrimaryExpression = number
      | "(" Expression ")"
      ;

The above grammar has two types of tokens (number and operator) and two implicitly defined tokens (left and right parenthesis). When then have two productions. `Expression` and `PrimaryExpression`. This distinction is necessary to avoid left recursion.

> *Note:* Remember, left recursion can only happen if you apply a production recursively without consuming a token. As a general rule, every application of a production should be preceded with at least one token.

If we were to implement the above grammar in the form of a recursive decent parser it would look something like this.

    void Expression() {
      PrimaryExpression();
      if (Accept(kOperator)) {
        Expression();
      }
    }

    void PrimaryExpression() {
      if (Accept(kNumber)) {
        return;
      }
      if (Accept(kLeftParenthesis)) {
        Expression();
        Expect(kRightParenthesis);
      }
    }

Writing the above kind of code is relatively straight forward. When you add syntax tree construction and error handling (which we don't have here) it gets tricky but still it's a straightforward methodology.

From what we have so far, we can construct a control flow graph.

![alt text](https://docs.google.com/drawings/d/1mrY-F6Afq4Ipbi-07Zp0MoJkSsSzjUlkmKt1H9cCkTA/pub?w=960&amp;h=600 "Recursive push/pop stack transitions")

This control flow graph can be represented in code like this:

    class ControlFlowNode {
      public:
        ParserTree* next_;
    };

    class AcceptNode : public ControlFlowNode {
      public:
        int token_;
        ParserTree* body_;
    };
    class ApplyNode : public ControlFlowNode {
      public:
        ParserTree* prod_;
    };
    class ReturnNode : public ControlFlowNode {
      public:
    };

    // for example:
    auto expr = new ApplyNode();

    auto primary_expr = new AcceptNode();
    primary_expr->token_ = kNumber;
    primary_expr->body_ = new ReturnNode();

    auto rparen = new AcceptNode();
    rparen->token_ = kRightParenthesis;
    rparen->next_ = new ReturnNode();

    auto eval = new ApplyNode();
    eval->prod_ = expr;
    primary_expr->next_ = rparen;

    auto lparen = new AcceptNode();
    lparen->token_ = kLeftParenthesis;
    primary_expr->next_ = lparen;

    expr->prod_ = primary_expr;

    auto expr2 = new ApplyNode();
    expr2->prod_ = expr;
    expr2->next_ = new ReturnNode();

    auto op = new AcceptNode();
    op->token_ = kOperator;
    op->body_ = expr2;
    op->next_ = new ReturnNode();

    expr->next_ = op;


Some important observations to point out.

* The first node of a production rule is associated with a name (or label) in the symbol table, that is the name of the production. This is important because if we want to apply this production somewhere else we have to reference this node. A reference of this kind is the equivalent of calling a top-level function in a typical recursive decent parser.
* An accept operation is a branching instruction, in other words, a conditional transfer of control to some other node. This transfer of control has to consume a token from the input stream. If there are no accept operations that can consume the next token from the input stream (and that token is not the end of file token) we have a syntax error.

Essentially we can emit (or generate) following byte code for our simple but expressive grammar (and it's runnable).

    3:
      return
    PrimaryExpression:
      accept   kTokenNumber
      beq      3
      jmp      4
    5:
      call     Expression
      jmp      6
    7:
      return
    6:
      accept   kTokenRightParenthesis
      beq      7
      jmp      8
    8:
      error
    4:
      accept   kTokenLeftParenthesis
      beq      5
      jmp      9
    9:
      return
    Expression:
      call     PrimaryExpression
      jmp      10
    11:
      call     Expression
      jmp      12
    12:
      return
    10:
      accept   kTokenOperator
      beq      11
      jmp      13
    13:
      return

### Virtual Execution System, Byte Code and Metadata

We define a program as byte code and metadata.

The byte code generated from the previous example is actually an interleaved byte code and metadata stream. When loading a program the metadata identifies the symbolic names for tokens and production rules. This allows us to render a more readable representation of our grammar (not just numbers). We then proceed with a single pass through the byte code to identify the offsets associated with labels.

The above process can be made simpler if we process the byte code and replace labels with actual offsets. Effectively rewriting the program.

The byte code generated by our code generation pass is only responsible for visiting every reachable node in the control flow graph.

Since we're going to run through the byte code many times it would be nice if we could represent the byte code in a way that allows us to easily skip through byte code that isn't relevant to what we're doing.
