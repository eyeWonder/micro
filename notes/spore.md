#spore

spore is the smallest unit of the Orcish ecosystem. If you can run spore, you can Orc a chip completely. 

I don't know much about landing spore in a hostile or indifferent environment. Penetration has ne'er been my hack. Once it's on, it's on.

spore teaches a processor three words of Low Orcish: these write RAM, read RAM, and read words from Flash. The arguments are literal data, as is the report: spore recognizes one of three commands in ASCII, all two chars long, and responds with a literal stream corresponding to the requested reads, or a zero byte for writes. 

An Orcish driver can use this logic to sweep the program memory for any self-modifying code calls. If they're present, we're in luck, if you can load the self-modify words from Flash you can use them. We can then dump the entire chip state into the serial stream, install, trampoline, self-reset and pwn away. 

##purpose

spore exists for two reasons. The first is to provide a bootloader where none exists. With 8k:512 targets, a serial library is a considerable expense: nearly 2.5k for the trinket softserial library. Add another k and a half minimum for a real bootloader, and we're hosed. 

the spore command itself should be a simple matter of putting the instructions to write flash somewhere in the flash bank, spore loads it and calls it against our implicit values, borking some of your memory. 

The other is to handshake into systems and replace them. For any chip in which the firmware upgrade path is known (see bunnie's excellent work here), we can inject a spore and use it to dump the entire chip through the terminal, wipe it, and send in a battle Orc to run test programs on the pinouts and report back. We preserve the existing firm on the master system, for later trampolining. 

The orc that executes spore to gently eviscerate a chip and orc it is called Walrus, after the Walrus and the Carpenter: 

    "It seems a shame," the Walrus said,
	"To play them such a trick...

