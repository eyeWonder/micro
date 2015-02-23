#Control Structures.

These are the core control structures of µlisp. 

## ? 

This is the familiar if: 

    (if (= foo bar) (baz) (bux))
    (? (= Fu Br) (Bz) (Bx)) 

If is a special form because the first lambda call does not affect the contents of `$`. 


## -> , <- , -< 

### -> : call and slot

`(-> A $1)` binds the result of evaluating `$1` to slot `A`. Strictly, it evaluates, binds the result to `$`, and binds this to `A`; Both `$` and `A`, which are different memory locations, now contain a copy.

if `A` is not a slot, `->` fails and does not evaluate the following form. 

### <- : slot and call

`(<- A $1)` binds `$` to `A` and evaluates `$1`. It is optional, being equivalent to `(-> A $) $1`.

### -< : slotgun

`(-< $0 $1 $2)` calls `$0`, causing `$` to take the return value, which we'll call `µ`. `$1` is called, `$` now contains a result. This is overwritten with `µ` and `$2` is called. `$` holds the value of this second evaluation when `-<` returns.

