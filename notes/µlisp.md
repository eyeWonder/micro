#µlisp

Inspired by bitlash. How small can we make something lisplike?

It's for microcontrollers, not for showing off. I'm using S-expressions because they are very, very easy to parse. 

Most of the ideas in here will prove incorrect, I expect. We'll see.

##Syntax

The numeric values `0-9` always make numbers. This is the most important rule: outside of string and comment contexts, these will always make numbers. I thought we'd use hex, I was wrong, it costs code at the expense of clarity and is the wrong tradeoff. Instead we'll use 'jank 64', hex 20-6f, which are space through o, to transmit dense information. 

The other characters are glyphs, and combine in three classes: runes, letters, and spaces. space-qua-space, newline, and comma are accepted as spaces, with unique semantics in the short form.  A single rune or letter followed by a space is a distinct command, and is the same as that rune or letter followed directly by a number, so `A 12` and `# f` are equal to `A12` and `#f`. Note that µlisp uses only and always hexadecimal values. 

Each rune is a parsing engine with particular rules associated with it. Some, notably `(` and `)`, may not be combined with other runes in any form. Others such as `;` for comment and `"` for string, consume larger amounts of the input stream, and may not be followed by other values to compose a symbol. `"` may be preceded by another glyph to compose a different symbol, `;` may not be. 

###Specifics


####May not begin or end a symbol

`space`, `,` `\n`

`(` `)` `"` `;`

###May not begin a symbol

`#` `$` `%` `'` `{` `[`

###May not end a symbol

`}` `]`

###May be found in any position

``~`@^&*_-+=|\:<>.?/``  

So we have 19 symbols which combine two ways with 52 letters: 19 * 52 * 2 = 1976

Plus the double and single forms of the symbols: 1976 + 104 = 2090

Plus the combinations with each other: 19 * 18 * 2 = 684

Plus eight symbols which combine one way each with 52 letters: 8 * 52 = 416

The dual forms and singles make sixteen more, 432 + 2090 + 684 = 3206 library functions available. 

Making `{[]}` bare would subtract a few hundred of those, but that would leave no user-extensible brace pairs, which would be unfortunate. I may compromise and make them rune-only combiners, so `-{` is valid but `A{` is not. This would avoid collision with userspace. I like this the more I think about it, it means there are 19 of each brace available since we can't possibly allow `}{` as a symbol in a sane language. The computer wouldn't mind...


####Consequences:

Short characters needn't have padding applied by the compressor. They execute as soon as they are encountered by the evaluator's decision tree, without needing to check the second slot for a space. Any bare character followed by any amount of whitespace is the same as that character directly followed by the next token.


### `(` and `)`

Our most complex state machine keeps paren count. Our prompt looks like `@k\n(`, an `@k` message (could also be `n@`, or something else), a newline, and the opening paren, which we would otherwise have to type. Like this:

    @k
    (

The first symbol after every `(` is either a lambda or a special form, what we call a command. Each `(` increments the counter, and tries to launch the leading symbol. If it fails, it does a failparse to bring the counter back down, and returns. If it can't failparse, it fails again, having consumed everything possible. 

Consequence: any command not known is simply ignored along with all enclosed information. This may happen silently, it may not, depending on bytes available. 

Each command is responsible for consuming its arguments, and sets a 'number of forms at this level' chunk to go with the depth bit. Each `)` checks that an appropriate number of forms were consumed, then initiates the closing action: this is what causes the effect of the lambda or special form. If it fails, it failparses its way out of the situation.



##Program Architecture 

**µlisp** exists to drive user functions in a sensible way on limited hardware. Much like bitlash but not C like. 

We have a tagged architecture, but no memory management. Everything except the stack is preallocated. 

###Slots

Values are stored in slots. Slots may either hold a 32 bit signed value, or a tagged value. There are an absolute number of slots for any given build, let's call it 32. They are called `A-Zg-l` in this case. 

There is a special slot called `$`, or the immediate value. This is the return value for any lambda. 

## Types

We have types in µlisp. numbers are 32 bit signed, and are nominally primitive. At least the slots may be typed: there is a type table, up to 32 bits width, and high bits indicate a typed use of slot. 

Types include: nil, lambdas, strings, arrays, alists, chars, and symbols. We may add more as appropriate. Most come in RAM, EEPROM, and Flash forms. Flash forms may need high-flash and low-flash, call it 21 types and the untyped value. Our system allows for 127 types, I believe. There are 24 bits available for data in a typed slot, on an architecture which offers 16 bit pointers for almost everything. 

### Lambdas.

A lambda takes the form `(\ (+ $0 $1))`. The arguments are implicit. To call a lambda, you must assign it a name, or put it in a slot: `(\: Pl (+ $0 $1))` or `(-> A (\ (+ $0 $1)))`. `->` puts the value of the immediate variable in the first argument, which must be a slot, after evaluating the second argument, which must be a lambda call. 

`$0` is equivalent to `$`, unless bound. 

Or you can just call it: `(\ (@ "boo"))($)`. 

Names may not be reassigned, slots may be reassigned at will. 

To call a lambda, use bindings: (-> B (Pl 2 C)). This rewrites the lambda as `(\ (+ 2 C))` and executes it, putting the result in `B`. Lambdas may have optional arguments, any `$[0-f]` (there are normally less than 16 implicit slots) not bound is assigned a value of nil. Additional arguments are neither assigned nor evaluated, they are passed over by the parser. The implicit slots may be assigned to, they actually exist if they don't get bound. As usual, there are some limited number of overflow slots available. These can get abused as local variables, but we call them implicit slots; they are in no sense closed over, for instance. µlisp makes most of the mistakes of ancient Lisp, on purpose. 


## Arrays

May we'll call these tuples. They are a string of the form `[A 12 #1 $]`, anything may be in them, but they are not inherently recursive. 

## Alists

Alists are associative lists, they are searched in the order given and return upon a first match. `{A 12, B #f, ? ??}`, commas are always optional. 

Note that neither arrays nor alists evaluate symbols. 

## Special forms

Y'know, needed. Or maybe not. Special costs words. Some forms are slightly more special than others...


##Short form

Because of our rather limited parsing capability, we can compress our syntax, and do when writing it out. ``(-> A (\ (+ $1 $2)))`, from earlier, can become `-̥>A\̥+̥$1$2;`. Not all whitespace can be extracted in this fashion. `;` is a comment in normal form and is reused to mark 3 closing parens. `,`, a whitespace, is reused for 2 closing parens, with `)` taking the usual value. The subscript dot indicates the high bit is set, which is how we show opening parens in short form. 

## Quotation

The character `'` quotes the following form. Instead of evaluating, the parser compresses the form and returns it. `('A)` returns A, `(Fu '(Br Bz) Bx)` calls `Fu` with `$0` containing `B̥rBz)` and $1 containing the result of evaluating `Bx`. I do not imagine there will be a quasiquote or unquote form, if you want to macro do it the hard way. You're on a µcontroller, please keep your code generation on the master side. Since we're using Sexprs this should be easy. 

We have a fixed amount of memory to work with. If you want to use deferred evaluation strategies, make a thunk and put it in EEPROM.

## Thunks. 

We can defer evaluation at any point, by making a thunk. A thunk is just a compressed string containing a lambda with the arguments bound. What `)` does is build a thunk from the lambda it's closing and then evaluates it. 

If instead we use the thunk form, ``` `` ``` perhaps? Like so: ```(-> A (`` (Fu (Br Bz) B)))```, then presuming `(BrBz)` produces 23, we end up with `A` bound to the value `(Fu23B)`. Note that in a thunk, any unbound implicit slot `$0-n` returns `~n` which we abbreviate with NAK, written `¬`. For partial application, use bind, written `` `: ``, as ``(`: Fu 12)``, which binds `$0` to 12 and leaves `$1` free to be filled. Note anything provided as a first argument to the new lambda will be evaluated but not used, since this is a simple string rewrite and `$0`, the first immediate value, is not present. It is good form to present `~n` for this purpose. Real partial application where the arguments get shifted would cost us code space. Note that this should work: ```(-> A (`` (Fu 12 '$0)))```. It binds the thunk, evaluating 12 to get 12 and `'$0` to get `$0`, which is rewritten over every instance of $1. `` `: `` is not core, being a simple case of thunk. 


