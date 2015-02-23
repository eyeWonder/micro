#Huffman optimization

We employ crude compression by removing whitespace, jacking the high bit for open parens, and collecting twoples and threeples of close parens. 

Here's an optimization for later: we want to express as much µlisp as possible in µlisp itself, as well as in C, so we can load additional vocabulary remotely. For small systems, we can write an algorithm that reads and writes pre-composed Huffman trees. It will have a constant size regardless of which character is returned by which prefix. 

We first optimize by including only Orc functions that are not longer than their generated ASM equivalent. Otherwise we call the C and enjoy the lack of tradeoffs. Over the remaining code, we perform Huffman compression, and check the resulting size of the code + the decompression/compression algorithm - the crude compression. If it's smaller, we include Huffman. To weight the comparison favorably, we should add something like 1/2 the user EEPROM capacity worth of code to the mix.