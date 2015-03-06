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

Following is a loose discussion considering implementation order and choice of tools.

##Dancer

Chuck says any program worth writing is worth writing several times over. Dancer is the first run.

Dancer's parser is looking pretty good and I'm almost ready to move to eval. After some thought, I've decided to write the whole shebang as an Arduino library and just pay the abstraction costs. Should still be fairly small. I can't handle weaning the project off Arduino at the moment, even though my only dependencies are the setup loop and the Serial library. 

Bitlash should be a good model for many parts of the system. I may even lift a function or two. Bitlash was the original inspiration for Orcish, especially this generation of it. 

##Prancer

Prancer is a pure-C variant. We'll probably use the stock bootloader and avr-libc for everything else. Linkable to Arduino ecosytems? 

Prancer should have self-write capability in the standalone version. 

Orc 1.0 is standalone, includes a bootloader that has Orc built in, and possibly loads directly using a jank64 translated version of .hex format. That's an important part of self-hosting, probably worthy of delaying 1.0 until we've achieved it. 

##Blitzen

ASM optimized bleeding edge code. I will probably write the core loop in ASM just for the lulz, as it is the third implementation and I should have the state machine sorted by then. Totally stripped loader based on micronucleus. Must run a usable subset of the Orc on Trinket hardware. 


##The Other Reindeer

Past this point, my ideas are seriously hazy. Dancer itself is a pretty good debugger and REPL. Dasher introduces various commands for stepping and profiling, possibly a suite of system query tools. I'm not entirely sure, to be honest.

The intention is that the reindeer work together, so a harness can include all or a few and run. You need one of dancer/prancer/blitzen to operate an Orc. comet and donner are a team, serving to optimize and perform true compilation on systems which support it. comet can work alone as a profiler for user use.

dasher is probably an optional subsystem of prancer which provides advanced text editing and debugging capabilities. Orcish being what it is, those functions are intimately intertwined.  

Truly, not sure what I'll do with the other two. vixen might provide a high-level language for programming in the medium, on decently roomy ARM systems. as for cupid, don't let the name fool you. We do want a 'war orc' that can work its way semi-autonomously through a system of chipz. That's cupid, a special package for Orcification of existing substrates. 