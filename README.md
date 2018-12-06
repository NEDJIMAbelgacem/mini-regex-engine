# mini-regex-engine :
small regex engine with basic stuff implemented in c++
# Regex syntax :
the regex syntax contains only 5 operators along with '(' and ')' to nest operations
  * '*' operator (iteration operator) : matches 0 or more sequences of the last regex string.
  * '' operator (concatenation operator) : concatenate 2 regexes. This operator is implicitly defined between characters.
  * '|' operator (union operator) : matches one of the 2 regexes on its left or right.
  * '+' operator to match one or more matches of the last character or the last expression inside parentheses.
  * '?' operator to match zero or one occurences of the last character or the last expression inside parentheses.
# Implementation details :
the implementation is not optimal and uses postfix formating of the regex
the compilation (generation of the finite state machine used to match text) is done as follow :
  * Generate the abstract syntax tree of the regular expression
  * Transform the tree to a finite state machine that contains 0 or 1 character length transitions
  * Construct a finite state machine that contains no 0 length cycles using Tarjan's strongly connected components algorithm
  * Transform the finite state machine to non determinisic finite state machine
I made a function to generate random regular expressions to test the regex matcher so the implementation is similar to the standard regex library of c++ .
# Additional features :
  * State machine visuzlisation using graphviz. (for debug purposes)
  * Abstract syntax tree visualisation. (for debug purposes)
  * Generated automaton visualistion. (for debug purposes)
  * Nothing worthy actually :(
# building :
  * install graphviz (look for the installer from their site and add %ROOT%\bin to envirnment variable).
  * build using MSVC2017.
  * use the classes and functions inside as you want.
# TODO :
  1) add other operators.
  2) use cmake :/
