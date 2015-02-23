#Floating Point Library

Vocabulary providing floating point functionality. 

A standard Orc cannot tell active floats from other numbers, due to limitations in the type table. Remember that types exist in Orcish, but are only checked by normal Orcs if there's a risk that something will go badly wrong, or for conditional purposes. 

`.` defines a float. $0 is the radix, $1 is the mantissa, $2 the exponent. As always, arguments are optional, and our default behavior of setting unbound values to nil serves us well here. Note that `(.)` makes a float from the immediate value, which is a slightly expensive no-op. 

`.+` etc as you'd expect. For AVRs float routines have to be handrolled, and I don't understand which operations can be performed on the integral representation without loss of meaning, so I'm not going too deep here. Just in general, `.` covers floats with many/most other runes, mostly in the head position. 