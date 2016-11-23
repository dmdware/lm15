

#ifndef LOCALMAP_H
#define LOCALMAP_H

#include "../platform.h"


//memory localizing hash map with changing hash function cipher key

//just keep a counter and rearrange its bits to give a non-linear result
//basically a sequence with a period of 2^32
//to get a different sequence, change which bits are switched

#if 0
https://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetNaive

Counting bits set, in parallel

	unsigned int v; // count bits set in this (32-bit value)
unsigned int c; // store the total here
static const int S[] = {1, 2, 4, 8, 16}; // Magic Binary Numbers
static const int B[] = {0x55555555, 0x33333333, 0x0F0F0F0F, 0x00FF00FF, 0x0000FFFF};

c = v - ((v >> 1) & B[0]);
c = ((c >> S[1]) & B[1]) + (c & B[1]);
c = ((c >> S[2]) + c) & B[2];
c = ((c >> S[3]) + c) & B[3];
c = ((c >> S[4]) + c) & B[4];
The B array, expressed as binary, is:
B[0] = 0x55555555 = 01010101 01010101 01010101 01010101
	B[1] = 0x33333333 = 00110011 00110011 00110011 00110011
	B[2] = 0x0F0F0F0F = 00001111 00001111 00001111 00001111
	B[3] = 0x00FF00FF = 00000000 11111111 00000000 11111111
	B[4] = 0x0000FFFF = 00000000 00000000 11111111 11111111
	We can adjust the method for larger integer sizes by continuing with the patterns for the Binary Magic Numbers, B and S. If there are k bits, then we need the arrays S and B to be ceil(lg(k)) elements long, and we must compute the same number of expressions for c as S or B are long. For a 32-bit v, 16 operations are used.
	The best method for counting bits in a 32-bit integer v is the following:

v = v - ((v >> 1) & 0x55555555);                    // reuse input as temporary
v = (v & 0x33333333) + ((v >> 2) & 0x33333333);     // temp
c = ((v + (v >> 4) & 0xF0F0F0F) * 0x1010101) >> 24; // count
The best bit counting method takes only 12 operations, which is the same as the lookup-table method, but avoids the memory and potential cache misses of a table. It is a hybrid between the purely parallel method above and the earlier methods using multiplies (in the section on counting bits with 64-bit instructions), though it doesn't use 64-bit instructions. The counts of bits set in the bytes is done in parallel, and the sum total of the bits set in the bytes is computed by multiplying by 0x1010101 and shifting right 24 bits.

	A generalization of the best bit counting method to integers of bit-widths upto 128 (parameterized by type T) is this:

	v = v - ((v >> 1) & (T)~(T)0/3);                           // temp
v = (v & (T)~(T)0/15*3) + ((v >> 2) & (T)~(T)0/15*3);      // temp
v = (v + (v >> 4)) & (T)~(T)0/255*15;                      // temp
c = (T)(v * ((T)~(T)0/255)) >> (sizeof(T) - 1) * CHAR_BIT; // count
See Ian Ashdown's nice newsgroup post for more information on counting the number of bits set (also known as sideways addition). The best bit counting method was brought to my attention on October 5, 2005 by Andrew Shapira; he found it in pages 187-188 of Software Optimization Guide for AMD Athlon™ 64 and Opteron™ Processors. Charlie Gordon suggested a way to shave off one operation from the purely parallel version on December 14, 2005, and Don Clugston trimmed three more from it on December 30, 2005. I made a typo with Don's suggestion that Eric Cole spotted on January 8, 2006. Eric later suggested the arbitrary bit-width generalization to the best method on November 17, 2006. On April 5, 2007, Al Williams observed that I had a line of dead code at the top of the first method.


	Count bits set (rank) from the most-significant bit upto a given position

	The following finds the the rank of a bit, meaning it returns the sum of bits that are set to 1 from the most-signficant bit downto the bit at the given position.
	uint64_t v;       // Compute the rank (bits set) in v from the MSB to pos.
unsigned int pos; // Bit position to count bits upto.
uint64_t r;       // Resulting rank of bit at pos goes here.

// Shift out bits after given position.
r = v >> (sizeof(v) * CHAR_BIT - pos);
// Count set bits in parallel.
// r = (r & 0x5555...) + ((r >> 1) & 0x5555...);
r = r - ((r >> 1) & ~0UL/3);
// r = (r & 0x3333...) + ((r >> 2) & 0x3333...);
r = (r & ~0UL/5) + ((r >> 2) & ~0UL/5);
// r = (r & 0x0f0f...) + ((r >> 4) & 0x0f0f...);
r = (r + (r >> 4)) & ~0UL/17;
// r = r % 255;
r = (r * (~0UL/255)) >> ((sizeof(v) - 1) * CHAR_BIT);
Juha Järvi sent this to me on November 21, 2009 as an inverse operation to the computing the bit position with the given rank, which follows.
	Select the bit position (from the most-significant bit) with the given count (rank)

	The following 64-bit code selects the position of the rth 1 bit when counting from the left. In other words if we start at the most significant bit and proceed to the right, counting the number of bits set to 1 until we reach the desired rank, r, then the position where we stop is returned. If the rank requested exceeds the count of bits set, then 64 is returned. The code may be modified for 32-bit or counting from the right.
	uint64_t v;          // Input value to find position with rank r.
unsigned int r;      // Input: bit's desired rank [1-64].
unsigned int s;      // Output: Resulting position of bit with rank r [1-64]
uint64_t a, b, c, d; // Intermediate temporaries for bit count.
unsigned int t;      // Bit count temporary.

// Do a normal parallel bit count for a 64-bit integer,                     
// but store all intermediate steps.                                        
// a = (v & 0x5555...) + ((v >> 1) & 0x5555...);
a =  v - ((v >> 1) & ~0UL/3);
// b = (a & 0x3333...) + ((a >> 2) & 0x3333...);
b = (a & ~0UL/5) + ((a >> 2) & ~0UL/5);
// c = (b & 0x0f0f...) + ((b >> 4) & 0x0f0f...);
c = (b + (b >> 4)) & ~0UL/0x11;
// d = (c & 0x00ff...) + ((c >> 8) & 0x00ff...);
d = (c + (c >> 8)) & ~0UL/0x101;
t = (d >> 32) + (d >> 48);
// Now do branchless select!                                                
s  = 64;
// if (r > t) {s -= 32; r -= t;}
s -= ((t - r) & 256) >> 3; r -= (t & ((t - r) >> 8));
t  = (d >> (s - 16)) & 0xff;
// if (r > t) {s -= 16; r -= t;}
s -= ((t - r) & 256) >> 4; r -= (t & ((t - r) >> 8));
t  = (c >> (s - 8)) & 0xf;
// if (r > t) {s -= 8; r -= t;}
s -= ((t - r) & 256) >> 5; r -= (t & ((t - r) >> 8));
t  = (b >> (s - 4)) & 0x7;
// if (r > t) {s -= 4; r -= t;}
s -= ((t - r) & 256) >> 6; r -= (t & ((t - r) >> 8));
t  = (a >> (s - 2)) & 0x3;
// if (r > t) {s -= 2; r -= t;}
s -= ((t - r) & 256) >> 7; r -= (t & ((t - r) >> 8));
t  = (v >> (s - 1)) & 0x1;
// if (r > t) s--;
s -= ((t - r) & 256) >> 8;
s = 65 - s;
If branching is fast on your target CPU, consider uncommenting the if-statements and commenting the lines that follow them.
	Juha Järvi sent this to me on November 21, 2009.
#endif


inline uint8_t CountSet(uint32_t v)
{
	uint8_t c;
	v = v - ((v >> 1) & 0x55555555);                    // reuse input as temporary
	v = (v & 0x33333333) + ((v >> 2) & 0x33333333);     // temp
	c = ((v + (v >> 4) & 0xF0F0F0F) * 0x1010101) >> 24; // count

	return c;
}

//position from rank
inline uint8_t SetPos(uint8_t r)
{
	/*
	The following 64-bit code selects the position of the rth 1 bit when counting from the left. 
	In other words if we start at the most significant bit and proceed to the right, counting the 
	number of bits set to 1 until we reach the desired rank, r, then the position where we stop 
	is returned. If the rank requested exceeds the count of bits set, then 64 is returned. The 
	code may be modified for 32-bit or counting from the right.
	*/

	//uint64_t v;          // Input value to find position with rank r.
	uint32_t v;
	//unsigned int r;      // Input: bit's desired rank [1-64].
	unsigned int s;      // Output: Resulting position of bit with rank r [1-64]
	//uint64_t a, b, c, d; // Intermediate temporaries for bit count.
	uint32_t a, b, c, d;
	unsigned int t;      // Bit count temporary.

	//static const int B[] = {0x55555555, 0x33333333, 0x0F0F0F0F, 0x00FF00FF, 0x0000FFFF};
	
	// Do a normal parallel bit count for a 64-bit integer,                     
	// but store all intermediate steps.                                        
	a = (v & 0x55555555) + ((v >> 1) & 0x55555555);
	//a =  v - ((v >> 1) & ~0UL/3);
	b = (a & 0x33333333) + ((a >> 2) & 0x33333333);
	//b = (a & ~0UL/5) + ((a >> 2) & ~0UL/5);
	c = (b & 0x0F0F0F0F) + ((b >> 4) & 0x0F0F0F0F);
	//c = (b + (b >> 4)) & ~0UL/0x11;
	d = (c & 0x00FF00FF) + ((c >> 8) & 0x00FF00FF);
	//d = (c + (c >> 8)) & ~0UL/0x101;
	//t = (d >> 32) + (d >> 48);
	t = (d >> 16) + (d >> 24);
	// Now do branchless select!                                                
	//s  = 64;
	s = 32;
	// if (r > t) {s -= 32; r -= t;}
	s -= ((t - r) & 256) >> 3; r -= (t & ((t - r) >> 8));
	t  = (d >> (s - 16)) & 0xff;
	// if (r > t) {s -= 16; r -= t;}
	s -= ((t - r) & 256) >> 4; r -= (t & ((t - r) >> 8));
	t  = (c >> (s - 8)) & 0xf;
	// if (r > t) {s -= 8; r -= t;}
	s -= ((t - r) & 256) >> 5; r -= (t & ((t - r) >> 8));
	t  = (b >> (s - 4)) & 0x7;
	// if (r > t) {s -= 4; r -= t;}
	s -= ((t - r) & 256) >> 6; r -= (t & ((t - r) >> 8));
	t  = (a >> (s - 2)) & 0x3;
	// if (r > t) {s -= 2; r -= t;}
	s -= ((t - r) & 256) >> 7; r -= (t & ((t - r) >> 8));
	t  = (v >> (s - 1)) & 0x1;
	// if (r > t) s--;
	s -= ((t - r) & 256) >> 8;
	//s = 65 - s;
	s = 33 - s;
	
	return s;
}

#if 0

//http://bits.stephan-brumme.com/lowestBitNotSet.html

unsigned int lowestBitNotSet(unsigned int x)
{
	return ~x & (x + 1);
}

unsigned int lowestBitNotSetSimple(unsigned int x)
{
	// to be consistent with lowestBitNotSet
	if (x == ~0)
		return 0;

	// shift right until finding a zero
	unsigned int result = 1;
	while ((x & result) != 0)
		result <<= 1; 
	return result;
}

// http://bits.stephan-brumme.com/lowestBitSet.html

 const unsigned int MultiplyDeBruijnBitPosition[32] =
02: {
03:   // precomputed lookup table
04:   0,  1, 28,  2, 29, 14, 24,  3, 30, 22, 20, 15, 25, 17,  4,  8,
05:   31, 27, 13, 23, 21, 19, 16,  7, 26, 12, 18,  6, 11,  5, 10,  9
06: };
07: 
08: unsigned int lowestBitSet(unsigned int x)
09: {
10:   // only lowest bit will remain 1, all others become 0
11:   x  &= -int(x);
12:   // DeBruijn constant
13:   x  *= 0x077CB531;
14:   // the upper 5 bits are unique, skip the rest (32 - 5 = 27)
15:   x >>= 27;
16:   // convert to actual position
17:   return MultiplyDeBruijnBitPosition[x];
18: }
19: 
20: unsigned int lowestBitSetSimple(unsigned int x)
21: {
22:   // x=0 is not properly handled by while-loop
23:   if (x == 0)
24:     return 0;
25: 
26:   unsigned int result = 0;
27:   while ((x & 1) == 0)
28:   {
29:     x >>= 1;
30:     result++;
31:   }
32: 
33:   return result;
34: }

//http://stackoverflow.com/questions/109023/how-to-count-the-number-of-set-bits-in-a-32-bit-integer

int NumberOfSetBits(int i)
{
     // Java: use >>> instead of >>
     // C or C++: use uint32_t
     i = i - ((i >> 1) & 0x55555555);
     i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
     return (((i + (i >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
}

// http://bits.stephan-brumme.com/countBits.html

01: unsigned int countBits(unsigned int x)
02: {
03:   // count bits of each 2-bit chunk
04:   x  = x - ((x >> 1) & 0x55555555);
05:   // count bits of each 4-bit chunk
06:   x  = (x & 0x33333333) + ((x >> 2) & 0x33333333);
07:   // count bits of each 8-bit chunk
08:   x  = x + (x >> 4);
09:   // mask out junk
10:   x &= 0xF0F0F0F;
11:   // add all four 8-bit chunks
12:   return (x * 0x01010101) >> 24;
13: }
14: 
15: unsigned int countBitsLoop(unsigned int x)
16: {
17:   unsigned int result = 0;
18:   // strip one set bit per iteration
19:   while (x != 0)
20:   {
21:     x &= x-1;
22:     result++;
23:   }
24:   return result;
25: }

#endif

// http://bits.stephan-brumme.com/countBits.html

inline uint32_t CtSet(uint32_t x)
{
	// count bits of each 2-bit chunk
	x  = x - ((x >> 1) & 0x55555555);
	// count bits of each 4-bit chunk
	x  = (x & 0x33333333) + ((x >> 2) & 0x33333333);
	// count bits of each 8-bit chunk
	x  = x + (x >> 4);
	// mask out junk
	x &= 0xF0F0F0F;
	// add all four 8-bit chunks
	return (x * 0x01010101) >> 24;
}

//http://bits.stephan-brumme.com/lowestBitNotSet.html

inline uint32_t LowestNotSet(uint32_t x)
{
	return ~x & (x + 1);
}

inline unsigned int lowestBitNotSetSimple(unsigned int x)
{
	// to be consistent with lowestBitNotSet
	if (x == ~0)
		return 0;

	// shift right until finding a zero
	unsigned int result = 1;
	while ((x & result) != 0)
		result <<= 1; 
	return result;
}

#define LOWEST0(x)			(~x & (x + 1))

inline bool Ahead(uint32_t current[32], uint32_t test[32])
{
	for(uint8_t i=31; i>=0; --i)
	//for(uint8_t i=0; i&31; ++i)
	{	
		if(test[i] > current[i])
			return true;
		if(test[i] < current[i])
			return false;
	}

	return false;
}

inline uint32_t TwoPow(uint32_t pow)
{
	uint32_t r = 1;

	for(uint32_t i=0; i<pow; ++i)
		r <<= 1;

	return r;
}

inline uint32_t Pow(uint32_t base, uint32_t pow)
{
	uint32_t r = 1;

	for(uint32_t i=0; i<pow; ++i)
		r *= base;

	return r;
}

//propagate up to expected output
//assumes lower levels are more significant masks and so counting changes higher levels more frequently
//but little changes from higher levels won't be propagated to all bits
//better to treat higher levels as more significant
//so change lower levels more frequently and try to keep higher levels as small as possible
//by "more significant" I meant the higher levels change less frequently, but the lower levels really have more impact on others
bool Try3(uint32_t mval[32], bool nolim, uint32_t out, int8_t level, uint32_t oval, uint32_t low /*, uint32_t high */, uint32_t* unmatch, uint32_t* expect);
void TestHash();

#endif