#µparse

The parser for µlisp needs to take as little stack frame as possible. Everything does. If I can do this in structured style I will, otherwise we'll `goto`. 

Each character in a lead position is responsible for doing the next step in the parse. `(` evaluates the next symbol as a command while bumping the level register, which starts at 1, and exits the parser when it reaches 0. Our prompt looks like `(`.

Special forms aside, `(` looks up the lambda bound to the value and then evaluates the arguments, left to right. If the arguments are lambda calls, they are "recursively" parsed until the first `)` is reached. The inner lambda is evaluated, the result written to `$`, and the value of the result is written into the evaluation string for the next lambda. When the 0 `)` is reached, the remaining string is evaluated, and the parser returns. 

Parsing is mostly counting. Special forms have certain shape requirements, for example, `(?(=0 1)(~n)(1))`, our if form, requires 3 forms to follow. If it has four or two, the if clause is badly formed, and the rewrite returns `~n` for nil, or `n!` to generate an error on evaluation, or `(n! @"message")` in exceptional circumstances to generate an error returning a message. We short-form `~n` to the ascii value 15, NAK (any number is always hex, last reminder) when thunking, since `nil` comes up more often in that context than as an input form. 
 
In low-RAM systems the amount of space available to write an evaluation string might be insufficient. We do compress as we go, using the high bit to mark `(` and never needing a close paren by definition, extracting all whitespace, and so on, still, we might run out. If so, we 'spill' the string over to EEPROM, if we have any. This pauses evaluation because EEPROM writing takes time. 

The above needs a good hosing. "parsing" is "evaluation" at the command line. With an intricate exception, read on.

Watch out for dromonyms like `=.` and `.=`. Especially with unfamiliar libraries. 

##Orc filter

The parser uses Orc filtration. µlisp used to be called Orcish and still is. 

The important rule is: anything not recognized is ignored. This always applies to µlisp components, which are allowed to fail silently by consuming their portion of the program without doing anything at all. 

Orcs recognize all printable ASCII characters. We consider tabs unprintable. UTF-8 may be squeezed into strings using `\[0-9]+` escaping, the only kind an Orc must support besides `\\`. Any other capability is in the standard library. 

Orcs also recognize the esc sequence and must minimally ignore all input until the next alphanumeric character. We use escape sequences for various practical and semantic purposes, so it's useful to parse these, but even the tiniest Orc should be able to filter that information out of an incoming stream of Orcish. 

Orcs are not guaranteed to have a consistent short form and it is never used to communicate. A set high bit makes a character out of band. Any other capability is found in the standard library, and may be emulated on a core level by manually sending or reading a byte to the appropriate pin. 

## The Very Special Parser

Each character is its own parser, responsible for setting paths through a 

