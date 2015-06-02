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

To understand recursive decent parsing we'll be looking at a source implementation and then translate that into a stack machine (pushdown automaton).

But first we need to define a grammar. We'll use our math grammar for this.

    number = ("0" - "9") + ;
    operator = ("+" | "-" | "*" | "/") + ;

    Expression = PrimaryExpression
      | PrimaryExpression operator Expression
      ;
    PrimaryExpression = number
      | "(" Expression ")"
      ;

The above grammar has two types of tokens (number and operator) but also introduces two implicitly defined tokens (left and right parenthesis).

When the have two productions. `Expression` and `PrimaryExpression`. This distinction is necessary to avoid left-recursion.

If we were to implement the above productions as a recursive decent parser it would look like this.

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

Writing the above kind of code is relatively straight forward. When you add syntax tree construction and error handling it gets messy but still it's a straightforward methodology. The important part here is to remember that each choice has to consume a token and when no choice ends up consuming a token we have an error.

The first thing we want to do here is to represent the choices that gets made in the form of a directed graph (or state machine).

We define our initial state as the "root" node and then our productions as such.

    root
      +Expression
    Expression
      +PrimaryExpression
      ?kOperator
        +Expression
        :return 3
      :return 1
    PrimaryExpression
      ?kNumber
        :return 1
      ?kLeftParenthesis
        +Expression
        !kRightParenthesis
        :return 3

The stack machine that we're trying to build looks like this.

![alt text](https://docs.google.com/drawings/d/1mrY-F6Afq4Ipbi-07Zp0MoJkSsSzjUlkmKt1H9cCkTA/pub?w=960&amp;h=600 "Recursive push/pop stack transitions")

If we add tree construction (a return value) things get more interesting (we'll disregard error handling for now) but in essence whatever we leave on the stack after running a production rule is the return value, if the production does not result in a return value we don't have a matching production at this point (this implies error).

Our parser graph has nodes and edges (state transitions).

    class ParserTree {
      public:
        int flags_; // control flow, kAccept, kExpect, kPush, kTransition, kReturn
        int token_;
        ParserTree* tran_;
        ParserTree* next_;
        void Accept(int token);
        void Expect(int token);
        ParserTree* Push(ParserTree*);
        void Append(ParserTree*);
    };

    ParserTree* root = new ParserTree();

    ParserTree* expr = new ParserTree(); // Expression

    ParserTree* pexp = new ParserTree(); // PrimaryExpression

    expr->Push(pexp); // create implicit ParserTree node that unconditionally transitions to pexp

    auto number = new ParserTree();
    number->Accept(kNumber);
    number->Return(0);

    pexp->Append(number);

    auto eval3 = new ParserTree();
    eval3->Expect(kRightParenthesis);

    auto eval2 = new ParserTree();
    eval2->Push(expr);

    eval2->Append(eval3);

    auto eval = new ParserTree();
    eval->Accept(kLeftParenthesis);
    eval->TransitionTo(eval2);
    eval->Return();

    pexp->Append(eval);

    auto expr2 = new ParserTree();
    expr2->Accept(kOperator);
    expr2->Push(expr);
    expr2->Return(3, "binary");

Some important observations to point out.

* The first node of a production rule is associated with a name (or label) in the symbol table, that is the name of the production. This is important because if we want to apply this production somewhere else we have to reference this node. A reference of this kind is the equivalent of calling a top-level function in a typical recursive decent parser.
* An accept operation is a branching instruction, in other words, a conditional transfer of control to some other node. This transfer of control has to consume a token from the input stream. If there are no accept operations that can consume the next token from the input stream (and that token is not the end of file token) we have a syntax error.

If we take a look at the above code we're getting very close to something that resembles an intermediate language (some kind of byte code).

Essentially we have the following operations.

Accept - conditional branch

The accept instruction is a conditional operation, it has an inner body in the same way that an if statement has. That condition is whether the symbol at the front of the input token stream matches a particular kind of token.

Transition - jump

An unconditional branch instruction to jump to some other piece of code.

Call - function call

Very similar to how a jump instruction occurs except that we track where we left of so that we know where we should return to.

Return - return from function call

Transfer control back to the calling code. This is only possible if the control was transferred to this code though the use of the function call instruction.

Our math grammar can then be expressed in the following instruction form.

    3:
      pop      1
      return
    PrimaryExpression:
      accept   kTokenNumber
      beq      3
      jmp      4
    5:
      call     1
      jmp      6
    7:
      pop      3
      return
    6:
      accept   kTokenRightParenthesis
      beq      7
      return
    4:
      accept   kTokenLeftParenthesis
      beq      5
      return
    Expression:
      call     PrimaryExpression
      jmp      8
    9:
      call     Expression
      jmp      10
    10:
      pop      3
      return
    8:
      accept   kTokenOperator
      beq      9
      return

A stack machine implementation for interpreting the above does not require a lot of code and when needed it can be further compiled down to microprocessor assembly.

The way we generate the IL is interesting in-itself and may be worth taking a closer look if you wondering about that.

### Virtual Execution Environment & Byte Code Representation

The Virtual Execution Environment evaluation stack is a stack of 32-bit signed integer types.

> **Note:** the byte code is emitted in the form of a msgpack array of integer types.

    0x01 <int32> | accept <token>

If the next symbol from the token input stream is equal to <token> push true on top of the evaluation stack otherwise push false

    0x02 <int32> | beq <target>

If the current value on top of the evaluation stack is non-zero (true) transfer control to the specified target instruction.

    0x03 <int32> | jmp <target>

Unconditionally transfer control to the specified target instruction.

    0x04 <int32> | call <target>

Push the return instruction address on top of the evaluation stack then transfer control to the specified target instruction.

    0x05 | return

Pop the return instruction address from the evaluation stack and transfer control to the target instruction.
