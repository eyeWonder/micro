#reindeer

We have different kinds of parsing engines for different circumstances. Orcish is so small we may as well keep rewriting it, but unlike Forth the limited semantic landscape **might** tend towards uniformity of semantics. This is always down to culture in the end. 

**dasher** is the step debugger and value inspector. 

**dancer** we write first. [Interactive REPL](dancer.md) with text editor and various optional bells and whistles.

**prancer** is intermediate, providing feedback on commands and a minimal command-driven editor.

**blitzen** is of course the fastest possible parse eval. Unfriendly to type into. blitzen is used for interorcish communication and light installs.  

**comet** is the profiler and tracer. 

**donner** is harnessed with comet, and is an optimizer that translates Orc to machine code. 

**vixen** and **cupid** are reserved. 

There will never be more than eight reindeer, because reindeer nybble at our memory and the sleigh has room for precisely 2^3 reindeer. Rudolph was a sore loser, but if you want to write him, he should blink a status LED in some kind of [ASCII-extended Morse code](http://ascii-table.com/morse-code.php)