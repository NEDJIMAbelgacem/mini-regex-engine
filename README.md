# mini-regex-engine
small regex engine with basic stuff implemented in c++

# regex syntax
the regex syntax contains only 3 operators along with '(' and ')' to nest operations
  * '*' operator (iteration operator) : matches 0 or more sequences of the last regex string.
  * '.' operator (concatenation operator) : concatenate 2 regexes.
  * '|' operator (union operator) : matches one of the 2 regexes on its left or right

# implementation details
the implementation is not optimal and uses postfix formating of the regex
the compilation (generation of the finite state machine used to match text) is done as follow :
  * generate the postfixed string of the regular expression
  * transform the string to a finite state machine that contains 0 or 1 character length transitions
  * construct a finite state machine that contains no 0 length cycles using Tarjan's strongly connected components algorithm
  * transform the finite state machine to non determinisic finite state machine
I made a function to generate random regular expressions to test the regex matcher and it worked fine.
If you find some invalid regex don.t hesitate to post an issue or contact me.
