#I Ching Cryptography

This has been bugging me for years, so I'll just write it out again.

I still feel like it's too easy but can't find the flaw. 

I Ching crypto, also known as Shitty Encryption, uses a one-time pad, call it a k. That's the first coin.

We encrypt some bytes using our one-time, and send them to a very very stupid processor. No room for non-shitty encryption. The plain text is our second coin.

When it runs out, we reuse the pad. Oh noes! Here's the thing: we reuse it once, and once only, to send another pad. That's the third coin. 

Our adversary has three coin tosses xored together. The first blend is coin 1 and coin 2. Coin 1 is a true flip, permuting an average of half the bytes of the first flip, and the adversary can't tell which. That's what a one-time pad is.

The second blend is two pieces of good entropy xored together. If we reused the one-time pad to send information, there would be a relationship between blend one and blend two. If we reuse it to send more entropy, there cannot be such a relationship: the second blend permutes an average of half the bits of the first blend. 

For extreme simplicity, let's say you're sending a stream of zeroes, one of great importance. Your first blend is a random mix of zeroes and ones, which we'll call the antipad.

For the adversary to confirm that it's receiving the antipad, they must know the pad. They don't know the pad, or it's not encryption, so they currently have no idea what's going on. 

The second blend is the anti-pad xor the new pad. The adversary knows full well (let's argue) that the new pad is under the old pad. If they knew they possessed the anti-pad, they could derive the pad, old and new. But they don't.

Now this is fragile. If they know that they're looking for zeroes, game over, they know they have the anti pad and they can keep stealing the pad. 

But we're only sending one bit of information. If the adversary possesses that bit, they don't need to decrypt anything. If we send two bits of information, they need both, and that scales.

We can also conceal our bit by wasting some entropy. Each time we send a bit, we proceed it with an entropy bit, and the rule is 'xor these two bits to get your bit', of course across our pad. To know the entropy bit, the enemy would have to know the pad, and they don't. So even if they know that the signal is all zeroes, all the time, they can't derive the pad from this knowledge. 

This is important because any usable communication may as well be all zeroes to a sophisticated adversary. Knowing that we're using English qualifies as having the whole bit. 

I can't find the flaw. I'm sure I'll be blown away when I do. It lacks any resilience against attacks that aren't MITM, but that's beside the point. You sure as fuck can't timing attack it. 

I think I'm getting burned here:

[Can't do it, won't work](http://crypto.stackexchange.com/questions/10735/re-using-parts-of-a-one-time-pad-to-encrypt-a-new-one) but I need to understand why...

Hmm. This isn't actually my scheme, which reuses the A block. Fuck I have other things to do. Get back to this. 


010101
000000

101010 <-- gets this

010101
100100
110001 <-- and this

101010
110001 

011011 <-- uh oh. that's the NOT of our second pad. Drat. 

