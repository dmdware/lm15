

#ifndef LOCALMAP_H
#define LOCALMAP_H

#include "platform.h"
#include "texture.h"

//memory localizing hash map with changing hash function cipher key

#define LOWEST0(x)			((~x) & (x + 1))
#define LOWEST1(x)			(x & ((~x) + 1))

#define HASHINT		uint8_t
//#define HASHINT	uint16_t
//#define HASHINT	uint8_t
#define HASHADDR	uint8_t
#define SHASHADDR	int8_t
#define TABINT		uint16_t
//#define HASHADDR	uint32_t
//#define SHASHADDR	int32_t
//#define HASHFIELD	uint32_t
//#define HASHCOUNT	uint8_t
//#define LOG2HASHBITS	2	//(log2(HASHBITS))
#define HASHBITS	8	//don't set odd
//4->0 6->1 1->2 3b 6l
//#define HASHBITS	16
//#define HASHBITS	(1<<LOG2HASHBITS)
#define MAXHASH		((((HASHINT)1)<<HASHBITS)-1)
//#define HASHCROP(x)	(x)
#define HASHCROP(x)	((x)&(MAXHASH))
#define HASHVALS			(MAXHASH+1)	//greater maps => less mask cases shared but more input possibilites eg x10=>1/8.5x,1.25x,a1=b40,a1=b400
//#define HASHVALS			((1<<HASHBITS))
//#define HASHVALS			(HASHBITS+3)
//#define HASHVALS				((1<<(HASHBITS))-1)
//#define HASHVALS			(HASHBITS-1)
#define HASHROTL(x,n)	HASHCROP((x<<n)|(x>>(HASHBITS-n)))
#define HASHROTR(x,n)	HASHCROP((x>>n)|(x<<(HASHBITS-n)))
//#define HASHLEVELS		(HASHVALS*2)	//4bit 4val 11lev
#define HASHLEVELS			(HASHBITS*3)
#define TABLEBITS			(HASHLEVELS*HASHBITS)
#define MAXTABLE	((((TABINT)1)<<TABLEBITS)-1)
#define TABCROP(x)	((x)&(MAXTABLE))
#define TABROTL(x,n)	TABCROP((x<<n)|(x>>(TABLEBITS-n)))
#define TABROTR(x,n)	TABCROP((x>>n)|(x<<(TABLEBITS-n)))
#define INWIDTHX		720
#define INWIDTHY		528	//video
//#define HASHLEVELS			((HASHBITS*HASHVALS-8))
//#define HASHLEVELS				HASHBITS
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


struct CaseMask;

//for Fill
struct Mask
{
	HASHINT mask, ovaldown, oval;
//	HASHINT mask[FITRESX*FITRESY*3];
//	HASHINT ovaldown[2][FITRESX*FITRESY*3];
	//HASHINT maskup;
//	HASHINT oval[FITERSX*FITRESY*3];
	//Mask* maskupp;
	//Case* casp;
	//CaseMask* maskupp;
};

struct Case;

//for Case
struct CaseMask
{
	//HASHINT mask;
	HASHINT ovaldown;
	//HASHINT maskup;
	//HASHINT oval;
	//CaseMask* maskupp;
	//Case* casp;
};

//all mval's levels match for c ch's
struct Case
{
	HASHINT mask;
	HASHINT refc;
	std::list<CaseMask>* chmasks;
	Case* caspp;
};

#if 0
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
#endif

struct Map
{
	HASHINT i;
	HASHINT o;
};

void UpFill2(HASHINT* mvals, HASHINT* ovals, HASHINT inval, HASHINT level, HASHINT stoplevel);

//#define FITRESX	20
//#define FITRESY	15
//#define FITRESX	2
//#define FITRESY	2
#define FITRESX	640
#define FITRESY	480
#define FITPIXELS	(FITRESX*FITRESY)
#define PIXELSINT	(uint64_t)

//#define SEG1LEVELS	(FITRESX*FITRESY*24)
//#define SEGPARTS	((SEGLEVELS+SEGLEVELS/2)*3)
//#define SEG1INPARTS	((INWIDTHX*INWIDTHY)/HASHBITS)
//#define SEG1LEVELS	((FITRESX*FITRESY)*2/HASHBITS)
//#define SEG1LEVELS	((int)sqrt(FITRESX*FITRESX+FITRESY*FITRESY)*2/HASHBITS*10)
//#define SEG1LEVELS	(2000/HASHBITS)
//#define SEG1LEVELS	(2889/HASHLEVELS/12)
#define SEGRGBPART	(24/HASHBITS)
//#define SEG1INPARTS	((12)*(SEG1LEVELS))
//every 2nd 2 parts are odd padded arrangements with 2 extra parts at edges
//so 7 parts repeat arrangements every 4 levels
//the whole part for a single video frame input is (SEG1INPARTS+SEG1INPARTS/2)
//because two subsequent frames are inputs, they are mixed over 3*(SEG1INPARTS+SEG1INPARTS/2)
//where 1/2 discarded bits are at the middle level
//first two (SEG1INPARTS+SEG1INPARTS/2)'s get info from the 2 frames, then mix at middle level
//#define SEGTHIRD	(SEG1INPARTS+SEG1INPARTS/2)
//#define SEGTHIRD	(SEG1INPARTS)
//#define SEG2INPARTS	(12*(SEG1LEVELS))
//#define SEGPARTS	(SEG1INPARTS*2+SEG2INPARTS)
//#define SEGLEVELS	(FITRESX*FITRESY*SEGRGBPART/2)
#define SEGLEVELS	30
extern int segis;
#define SEGIS		segis //(FITRESX*FITRESY*SEGRGBPART*FITRESX*FITRESY*SEGRGBPART/20)
#define MAXSEGIS	(SEGLEVELS*3*FITRESX*FITRESY)
//extern HASHINT segments[SEGPARTS][HASHLEVELS];
//#define SEGIS	(HASHBITS*HASHLEVELS*SEGPARTS/64+1)
//#define SEGIS	(HASHLEVELS*SEGPARTS)
//extern uint64_t segments[SEGIS];
extern HASHINT segments[MAXSEGIS];
extern std::list<Map> mappings[MAXSEGIS];

#if 0
#define SEGMASK(segi,lev) ( (segments[(lev+HASHLEVELS*segi)*HASHBITS/64]& \
				(((uint64_t)3)<<((lev+HASHLEVELS*segi)*HASHBITS+0)%64)) | \
			(segments[(lev+HASHLEVELS*segi)*HASHBITS/64+1]& \
				(((uint64_t)3)<<((lev+HASHLEVELS*segi)*HASHBITS+0)%64)) )
#endif

#define SEGLOOK(segi,lev)	( (segments[(lev+HASHLEVELS*segi)*HASHBITS/64]& \
				(((uint64_t)3)<<((lev+HASHLEVELS*segi)*HASHBITS)%64)) )



void Up(HASHINT in, HASHINT* inval, HASHINT* inmask);
void TestHash();
//HASHINT Hash(HASHINT* mval, HASHINT oval, HASHINT oval2, HASHADDR level, bool echo=false);

void DownFill(HASHINT* mvals, std::list<HASHINT>* maps_ivals, HASHINT inval, HASHINT level, HASHINT stoplevel, HASHINT mci, HASHINT nmc, HASHINT nlevels);

bool UniqueMask2(CaseMask a, CaseMask b);

bool Down2(
	HASHINT* out,
	HASHINT* in,
	HASHINT* mval,
	HASHINT nlevels,
	HASHINT nmc
		  );
bool Down4(
	HASHINT* out,
	HASHINT* in,
	HASHINT* mval,
	HASHINT nlevels,
	HASHINT nmc
		  );
bool Down4Up(
	HASHINT* out,
	HASHINT* in,
	HASHINT* mval,
	HASHINT nlevels,
	HASHINT nmc
		  );

void pb(char* c, HASHINT i);
void pb22(char* c, HASHINT i);
void pb2(char* c, HASHINT t, HASHINT m);
void Do();

void Do2();

void Set(std::list<HASHINT>* i, std::list<HASHINT>* o);
void Get(std::list<HASHINT>* i, std::list<HASHINT>* o);

bool Narrow2(std::list<Mask>* a, std::list<Mask>* b);

void FillUp();

void Fill(HASHINT ival0[FITRESX*FITRESY*3], 
		  HASHINT ival1[FITRESX*FITRESY*3], 
		  HASHINT iset[FITRESX*FITRESY*3], 
		  int fullbin, 
		  std::list<Mask>& masks, 
		  HASHINT mset[FITRESX*FITRESY*3], 
		  HASHINT oval[FITRESX*FITRESY*3], 
		  HASHINT mval[FITRESX*FITRESY*3],
		  int levelsegs);

void Hash(HASHINT* ovalup, HASHINT* mval, HASHINT* oval, HASHINT* oval2, HASHADDR level);

void Hash(HASHINT* ovalup, HASHINT mval, HASHINT oval, HASHINT oval2, HASHADDR level);

void Misbits(HASHINT oval[FITRESX*FITRESY*3],
			 HASHINT oval2[FITRESX*FITRESY*3],
			 HASHINT* misbits,
			int levelsegs);

bool Less(HASHINT a[FITRESX*FITRESY*3],
		  HASHINT b[FITRESX*FITRESY*3],
		  int levelsegs);

void TeachSeg(LoadedTex* prevltex, LoadedTex* ltex, int fi, LoadedTex* outtex);
void FeedOut(LoadedTex* prevltex, LoadedTex* ltex, int fi, LoadedTex* outtex);

bool CompareMask(Mask& a, Mask& b);

void Shift(LoadedTex* tex, int amt);

#endif
