# Parserlib

A c++17 recursive-descent parser library that can parse left-recursive grammars.

## Table Of Contents
[Introduction](#introduction)

[Using the Library](#using-the-library)

[Writing a Grammar](#writing-a-grammar)

[Invoking a Parser](#invoking-a-parser)

[Non-Left Recursion](#non-left-recursion)

[Left Recursion](#left-recursion)

[Customizing a Parser](#customizing-a-parser)

[Simple Matches](#simple-matches)

[Tree Matches](#tree-matches)

[Resuming From Errors](#resuming-from-errors)

## <a id="Introduction"></a>Introduction

Parserlib allows writing of recursive-descent parsers in c++ using the language's operators in order to imitate <a src="https://en.wikipedia.org/wiki/Extended_Backus%E2%80%93Naur_form">Extended Backus-Naur Form (EBNF)</a> syntax.

The library can handle left recursion.

Here is a Calculator grammar example:

```cpp
extern Rule<> add;

const auto val = +terminalRange('0', '9');

const auto num = val
               | '(' >> add >> ')';

Rule<> mul = mul >> '*' >> num
           | mul >> '/' >> num
           | num;

Rule<> add = add >> '+' >> mul
           | add >> '-' >> mul
           | mul;
```

The above grammar is a direct translation of the following left-recursive EBNF grammar:

```
add = add + mul
    | add - mul
    | mul
    
mul = mul * num
    | mul / num
    | num
    
num = val
    | ( add )
    
val = (0..9)+
```

## Using the Library

The library is available as headers only, since every class is templated.

In order to use it, you have to have the path to its root include folder in your project's include folder list.

Then, you have to either include the root header, like this:

```cpp
#include "parserlib.hpp"
```

Or use the various headers in the subfolder 'parserlib'.

All the code is included in the namespace `parserlib`:

```cpp
using namespace parserlib;
```

## Writing a Grammar

A grammar can be written as a series of parsing expressions, formed by operators and by functions that create parser objects.

### Terminals

The most basic parser is the `TerminalParser`, which is used to parse a terminal. In order to write a terminal expression, the following code must be written:

```cpp
terminal('x')
terminal("abc")
```

*The terminals in this library are by default of type `char`, but they can be customized to be anything.*

Other types of terminal parsers are:

```cpp
terminalRange('a', 'z') //parses all values between 'a' and 'z'.
terminalSet('+', '-') //parses '+' or '-'.
```

### Sequences

Terminals can be combined in sequences using the `operator >>`:

```cpp
const auto ab = terminal('a') >> terminal('b');
const auto abc = ab >> terminal('c');
```

In order to parse a sequence successfully, all members of that sequence shall parse successfully.

### Branches

Expressions can have branches:

```cpp
const auto this_or_that = terminal("this")
                        | terminal("that");
```

Branches are followed in top-to-bottom fashion.
If a branch fails to parse, then the next branch is selected, until a branch is found or no more branches exist to follow.

### Loops

- The `operator *` parses an expression 0 or more times.
- The `operator +' parses an expression 1 or more times.

```cpp
+(terminalRange('0', '9')) //parse a digit 1 or more times.
```

### Optionals

A parser can be made optional by using the `operator -`:

```cpp
-terminalSet('+', '-') >> terminalRange('0', '9') //parse a number; the sign is optional.
```

### Conditionals

- The `operator &` allows parsing an expression without consuming any tokens; it returns true if the parsing succeeds, false if it fails. It can be used to test a specific series of tokens before parsing.
- The `operator !` inverts the result of a parsing expression; it returns true if the expression returns false and vice versa.

```cpp
!terminalSet('=', '-') >> terminalRange('0', '9') //parse an integer without a sign.
```

### Matches

- The `operator ==` allows the assignment of a match id to a production; [the created match does not have any children](#simple-matches).
- The `operator >=` allows the assignment of a match id to a production; [the created match has children matches](#tree-matches).


```cpp
(-terminalSet('+', '-') >> terminalRange('0', '9')) == std::string("int")
```

## Invoking a Parser

In order to invoke a parser, the appropriate `ParseContext` instance must be created.

```cpp
//declare a grammar
const auto grammar = (-terminalSet('+', '-') >> terminalRange('0', '9')) == std::string("int");

//declare an input
std::string input = "123";

//declare a parse context over the input
ParseContext<> pc(input);

//parse
const bool ok = grammar(pc);

//iterate over recognized matches
for(const auto& match : pc.matches()) {
    if (match.id() == "int") {
        const auto parsedString = match.content();
        //process int
    }
}
```

## Non-left Recursion

Rules allow the writing of recursive grammars.

```cpp
//whitespace
const auto whitespace = terminal(' ');

//integer
const auto integer = terminalRange('0', '9');

//forward declaration of recursive rule
extern Rule<> values;

//value; it is recursive
const auto value = integer 
                 | terminal('(') >> values >> terminal(')');

//rule
Rule<> values = value >> whitespace >> values;
```

## Left Recursion

The library can parse left recursive grammars.

```cpp
//the recursive rule
extern Rule<> expression;

//and integer is a series of digits
const auto integer = +terminalRange('0', '9');

//a value is either an integer or a parenthesized expression
Rule<> value = integer 
             | '(' >> expression >> ')';

//multiplication
Rule<> mul = mul >> '*' >> value
           | mul >> '/' >> value
           | value;
           
//addition
Rule<> add = add >> '+' >> mul
           | add >> '-' >> mul
           | mul;
           
//the root rule
Rule<> expression = add;                      
```

## Customizing a Parser

The class ParseContext is a template and has the following signature:

```cpp
template <class SourceType, class MatchIdType, class SourcePositionType> class ParseContext;
```

It allows customizing the source type, the match id type and the source position type.

### Customizing the source type

By default, a ParseContext instance will use an `std::string` as an input source. But this can be changed to accomodate any STL like container.

For example, the source can be a static array of integers:

```cpp
ParseContext<std::array<int, 1000>> pc(input);
```

### Customizing the match id type

The default match id type is `std::string`, but usually it shall be an integer or an enumeration. It's also good for performance reasons to replace `std::string` with a numeric value, since match ids are created and destroyed as parsing is performed.

Example:

```cpp
ParseContext<std::string, int> pc(input);
```

### Customizing character processing

The parse context's parameter named '`SourcePositionType' allows the customization of character processing:
- customizing comparison of elements, for example in order to implement case insensitive parsing.
- providing extra information regarding the source, for example line and oclumn numbers.
- customizing the newline character sequence.

The library already provides two classes for the above:
- class `SourcePosition<class SourceType, bool CaseSensitive>` is the most basic class that just contains an iterator for the current position; it allows for statically using either case sensitive or case insensitive parsing.
- class `LineCountingSourcePosition<class SourceType, bool CaseSensitive, class NewlineTraits>` extends the class `SourcePosition` with line and column information, and it also allows the specification of newline sequence, which, by default, is implemented by class `DefaultNewlineTraits` that recognizes the character `\n` as the newline separator.

Examples:

```cpp
//case insensitive parsing
ParseContext<std::string, int, SourcePosition<std::string, false>> pc(input);

//case sensitive parsing with line counting
ParseContext<std::string, int, LineCountingSourcePosition<std::string>> pc(input);

//case insensitive parsing with line counting and custom newline traits
ParseContext<std::string, int, LineCountingSourcePosition<std::string, false, CustomNewlineTraits>> pc(input);
```

## Simple Matches

The `operator ==` allows the creation of a match, when an expression parses successfully. The right hand side should be an expression which evaluates to the match id expected by the parse context. Example:

```cpp
enum TYPE {
    A, B, C
};

const auto a = terminal('A') == A;
const auto b = terminal('B') == B;
const auto c = terminal('B') == C;
const auto grammar = a >> b >> c;

std::string input = "ABC";
ParseContext<std::string, Type> pc(input);

const bool ok = grammar(pc);
for(const auto& match : pc.matches()) {
    std::cout << match.content() << " = " << match.id() << std::endl;
}
```

The above produces the output:

```
A = 0
B = 1
C = 2
```

## Tree Matches

The `operator >=` allows the creation of a match, like the `operator ==`, with a difference: all matches created within the context of the expression are placed as children matches.

This allows matches to also be trees, instead of a flat list. 

In the following example, an IP4 address is returned as a tree match, with the following structure:

```
IP4_ADDRESS
    HEX_BYTE
        HEX_DIGIT
        HEX_DIGIT
    HEX_BYTE
        HEX_DIGIT
        HEX_DIGIT
    HEX_BYTE
        HEX_DIGIT
        HEX_DIGIT
    HEX_BYTE
        HEX_DIGIT
        HEX_DIGIT
```

Here is the code:

```cpp
enum TYPE {
    ZERO,
    ONE,
    TWO,
    THREE,
    FOUR,
    FIVE,
    SIX,
    SEVEN,
    EIGHT,
    NINE,
    A,
    B,
    C,
    D,
    E,
    F,
    HEX_DIGIT,
    HEX_BYTE,
    IP4_ADDRESS
};

const auto zero  = terminal('0') == ZERO ;
const auto one   = terminal('1') == ONE  ;
const auto two   = terminal('2') == TWO  ;
const auto three = terminal('3') == THREE;
const auto four  = terminal('4') == FOUR ;
const auto five  = terminal('5') == FIVE ;
const auto six   = terminal('6') == SIX  ;
const auto seven = terminal('7') == SEVEN;
const auto eight = terminal('8') == EIGHT;
const auto nine  = terminal('9') == NINE ;

const auto a = terminal('A') == A;
const auto b = terminal('B') == B;
const auto c = terminal('C') == C;
const auto d = terminal('D') == D;
const auto e = terminal('E') == E;
const auto f = terminal('F') == F;

const auto hexDigit = (zero | one | two | three | four | five | six | seven | eight | nine | a | b | c | d | f) >= HEX_DIGIT;

const auto hexByte = (hexDigit >> hexDigit) >= HEX_BYTE;

const auto ip4Address = (hexByte >> terminal('.') >> hexByte >> terminal('.') >> hexByte >> terminal('.') >> hexByte) >= IP4_ADDRESS;

const std::string input = "FF.12.DC.A0";

ParseContext<std::string, TYPE> pc(input);
using Match = typename ParseContext<std::string, TYPE>::Match;

const bool ok = ip4Address(pc);

assert(ok);
assert(pc.matches().size() == 1);

const Match& match = pc.matches()[0];

std::stringstream stream;
stream << match.children()[0].children()[0].content();
stream << match.children()[0].children()[1].content();
stream << '.';
stream << match.children()[1].children()[0].content();
stream << match.children()[1].children()[1].content();
stream << '.';
stream << match.children()[2].children()[0].content();
stream << match.children()[2].children()[1].content();
stream << '.';
stream << match.children()[3].children()[0].content();
stream << match.children()[3].children()[1].content();
const std::string output = stream.str();
std::cout << output;
```

The above prints the input, which is the value `FF.12.DC.A0`.

## Resuming From Errors

In order to resume from errors, the special `operator ~()` can be used to create an `error resume point`.

An `error resume point` shall be combined with `operator >>()` to create a sequence of parsers, in which the parsers before the `error resume point` may create an error, and then the `error parser` will try to resume parsing from the `error resume point`.

Here is an example of parsing a terminal enclosed in single quotes:

```cpp
const auto ws = *terminal(' ');
const auto letter = terminalRange('a', 'z') | terminalRange('A', 'Z');
const auto digit = terminalRange('0', '9');
const auto character = letter | digit;
const auto terminal_ = ('\'' >> *(character - '\'') >> ~terminal('\'')) == "terminal";
const auto grammar = ws >> *(terminal_ >> ws);
```

If an error happens when parsing a terminal, then the parser will look for the single quote symbol `\'` in order to continue parsing.
