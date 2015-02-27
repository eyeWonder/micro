#Slots

Instead of user variables, we offer slots. There are generally 16 of them, `A-P`, and they're 32 bits width. They have a type, stored in a two-bit (probably) type table for each slot.

Slots can be accessed as the full width, and often are, this is simply `A`. They can also be addressed as 16 bit sub-slots, called `A<` for the low and `A>` for the high word. We also offer byte addressing, from low to high: `aa`, `aA`, `Aa`, and `AA`. 

As mentioned, slots are typed: the table tells which type, and additional type information is often found in the high byte of the slot. The immediate variable `$` is 32 bits and typed; assignment of the full value to a slot will transfer the type. Transferring a subslot (e.g. `(-> aa 23)`) has **no effect** on the type of the slot, unless it overwrites type information (this will never happen except on assignment to `AA`). Even then, it will not affect the type table unless the full slot is assigned to. This is potentially a useful property, if you're keeping a bunch of reljumps around you can swap them in and out of the low word of a slow and not have to muck with the type info, or if you have a command you may change the bytes of it. 