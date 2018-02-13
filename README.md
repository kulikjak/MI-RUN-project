# XE interpreted language

XE is dynamically typed, garbage collected and interpreted language written as a University assignment. Syntax is somehow similar to the of Pascal 

Xe works with several data types: Booleans, Integers (with different sizes), Strings and Undefined. Most of the expression operations work only with integers (except exact comparison).

Floating point types are not supported.

Global variables are also supported but in the current state of the language, they are not interesting (because there is only one scope for entire program).

XE doesn't have standard arrays but it supports similar structure called aggregators. Every single statement has a return value and each of these can be pushed into one of 10 aggregators. These can then be reduced using built-in functions. As of now, only integer types can be used with aggregator functions.

User-defined functions are not supported.

### Installation
To install interpreter and language simply download this repository and run `make Lang` (`GNUmake`). It requires no additional libraries or special compiler extensions (it is written in standard C).

### Usage
You can find example files in `programs` folder. Each of them shows different capability of XE programming language. You can also find grammar of the parsing in `docs` folder.

Each program must have one main function enclosed in `block main` and `end` statements. Statements are divided by newlines - these is no such things ase semicolons or dots.

####Statement list

  - assignments (`a := 6`)
  - read and write statements (`read into b`, `write c`)
  - increment or decrement (`increment d`, `decrement e by 6`)
  - assertions (`assert 6 = 2`)
  - if condition (`if j < 6 begin .... end`)
  - while loops (`while i = 2 begin .... end`)
  - do loops (`do 20 times begin .... end`)
  - expressions

Variables are created automatically and doesn't have to be declared before. Only exception is obviously global variable, which is created with `global variable X` statement before main function.

Read statement can read integers and strings. Write statement can print out anything except undefined variables.

Increment and decrement nodes do exactly what you would expect...

Assetion will stop program execution if given expression evaluates to `false`.

Conditions and while loop are pretty much the same in their syntax and again are used in the same way as in other programming languages.

Do loop will execute its body exactly `n` times. There is no way to jump out of the execution. It works with integer variables and numbers.

Expressions are the same as in C wih several exceptions. Comparison is done with only one `=`. Also calling aggregator functions uses different syntax. Each function accepts aggregator and optionally output variable.

Aggregators right now support these functions:
  - sum(x -> y)
  - average(x -> y)
  - multiply(x -> y)
  - clear(x)

### Author
* Jakub Kulik, <kulikjak@fit.cvut.com>

