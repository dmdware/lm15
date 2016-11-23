

#ifndef LOCALMAP_H
#define LOCALMAP_H

#include "../platform.h"

//memory localizing hash map with changing hash function cipher key

#define LOWEST0(x)			(~x & (x + 1))
#define LOWEST1(x)			(x & (~x + 1))

#define HASHINT		uint64_t
//#define HASHINT	uint16_t
//#define HASHINT	uint8_t
#define HASHADDR	uint16_t
#define SHASHADDR	int16_t
//#define HASHADDR	uint32_t
//#define SHASHADDR	int32_t
//#define HASHFIELD	uint32_t
//#define HASHCOUNT	uint8_t
#define LOG2HASHBITS	2	//(log2(HASHBITS))
#define HASHBITS	16
//4->0 6->1 1->2 3b 6l
//#define HASHBITS	16
//#define HASHBITS	(1<<LOG2HASHBITS)
#define MAXHASH		((((HASHINT)1)<<HASHBITS)-1)
//#define HASHCROP(x)	(x)
#define HASHCROP(x)	((x)&(MAXHASH))
#define HASHVALS			3
//#define HASHVALS			((1<<HASHBITS))
//#define HASHVALS			(HASHBITS+3)
//#define HASHVALS				((1<<(HASHBITS))-1)
//#define HASHVALS			(HASHBITS-1)
#define HASHROTL(x,n)	HASHCROP((x<<n)|(x>>(HASHBITS-n)))
#define HASHROTR(x,n)	HASHCROP((x>>n)|(x<<(HASHBITS-n)))
//#define HASHLEVELS		(HASHVALS*2)	//4bit 4val 11lev
//#define HASHLEVELS			((HASHBITS*HASHVALS))
#define HASHLEVELS				HASHBITS
//#define HASHLEVELS			(1<<HASHBITS)
//#define HASHLEVELS		(HASHBITS*(1<<(LOG2HASHBITS*LOG2HASHBITS))/(HASHBITS*HASHBITS))
//#define HASHLEVELS		((1<<(LOG2HASHBITS*(LOG2HASHBITS))))
//#define HASHLEVELS	(HASHBITS)	//2 works for 1 bits
//#define HASHLEVELS	(HASHBITS<<2)	//9 works for 3 bits
//#define HASHLEVELS	((HASHBITS<<1))	//9 works for 3 bits
//#define HASHLEVELS		((1<<HASHBITS)+1)
//#define HASHLEVELS	(4)
//#define HASHLEVELS	((HASHBITS<<1)+4)
//#define HASHLEVELS	((HASHBITS*HASHVALS)+HASHVALS)	//works for 3 bits
//#define MAXJUMP			(1<<3)
//#define HASHLEVELS		(7)
//#define HASHLEVELS		(3+HASHBITS)	//log2(MAXJUMP)+HASHBITS
//#define HASHLEVELS	(HASHBITS<<4+1)	//works for 3 bits
//#define HASHLEVELS		(HASHBITS*HASHBITS)
//#define HASHLEVELS		(HASHBITS*HASHBITS*HASHBITS)
//#define HASHLEVELS		((1<<HASHBITS)+1)
//#define HASHLEVELS			((1<<(HASHBITS*HASHBITS))+1)

/*
what's the least possible mask you could have (greater than x) if you had to get this output, given any input?

fails at bottom, sends signal up to choose next possible mask
*/

struct Mask
{
	HASHINT mask;
	HASHINT ovaldown;
	HASHINT maskup;
};

inline bool Ahead(HASHINT current[HASHLEVELS], HASHINT test[HASHLEVELS])
{
	for(SHASHADDR i=(HASHLEVELS-1); i>=0; --i)
	//for(uint8_t i=0; i&31; ++i)
	{	
		if(test[i] > current[i])
			return true;
		if(test[i] < current[i])
			return false;
	}

	return false;
}

struct Tried
{
	HASHINT tried;	//single
	HASHINT mask;	//both
};

//00 00
//10 01
//8b

//0000
//0001
//1000
//1001

//000m
//100m

//m0 0m
//4b

//m00m



//00 00
//11 11
//8b

//0000
//0011
//1100
//1111
//16b

//8b




//blob1
//00 00
//11 11

//blob2
//10 00
//11

//ublob
//10 00

//sblob
//11 00

//uublob
//10


//max entries * bits per entry = bit width of number * channels * 2 * bit width = max bits
//4 bit width * 2 * 1 ch = 8 max entries * 4 bits = 32 max bits
//2 bit width * 2 * 2 ch = 8 * 2 bits = = 16 max bits

//64*100 bit width * 2 * 1 ch * 64*100 bit width = 12800 entries * 6400 bits =  81920000 max bits = 78.125 MB
//64 bit width * 2 * 100 ch * * 64 bit width = 12800 entires * 64 bits = 819200 max bits = 800 kB



bool DidTry(std::list<Tried>* t, HASHINT s, HASHINT setmask);
void AddTried(std::list<Tried>* t, HASHINT s);
bool DidTry(std::list<Tried>* t, HASHINT s);
void AddTried(std::list<Tried>* t, HASHINT s, HASHINT ch);
bool DidTry(std::list<Tried>* t, HASHINT s, HASHINT setmask, HASHINT ch);
bool Down(std::list<Mask>* masksup, 
		  HASHINT mval[HASHLEVELS], 
		  HASHINT* maskup, 
		  HASHADDR level, 
		  HASHINT* in, 
		  bool lowlim, 
		  HASHINT* upin, 
		  HASHINT* upinmask, 
		  HASHINT nmc,
		  std::list<Tried>* tried);
void Up(HASHINT in, HASHINT* inval, HASHINT* inmask);
void TestHash();
HASHINT Hash(HASHINT mval[HASHLEVELS], HASHINT oval, HASHADDR level);
void UpFill(HASHINT in, std::list<HASHINT>* invals, HASHINT inval, HASHINT inmask);
void UpFill(HASHINT in, std::list<HASHINT> upvals[HASHLEVELS]);

#endif