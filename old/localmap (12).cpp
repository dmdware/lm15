

#include "../utils.h"
#include "../debug.h"
#include "localmap.h"


void TestHash()
{
#if 1
	uint64_t t = GetTicks();

	//for(int times=0; times<10; ++times)
	{
		HASHINT m[HASHLEVELS];
		HASHINT m2[HASHLEVELS];
		memset(m, 0, HASHLEVELS * sizeof(HASHINT));
		memset(m2, 0, HASHLEVELS * sizeof(HASHINT));

		struct Map
		{
			HASHINT i;
			HASHINT o;
		};

		std::list<Map> map;

		uint32_t r = GetTicks();
		
		//for(HASHINT o=0; o<pow(2,HASHBITS)-1; ++o)
		//for(HASHINT o=0; o<pow(2,HASHBITS); ++o)
		//for(HASHINT o=0; o<pow(2,HASHBITS-2)-1; ++o)
		//for(HASHINT o=0; o<HASHBITS-1; ++o)
		for(HASHINT o=0; o<HASHVALS; ++o)
		//for(HASHINT o=0; o<0; ++o)
		{
			while(true)
			{
				r *= 0x12345;
				r += 98765;

				//HASHINT i = 12;
				//HASHINT i = 23;
				//HASHINT i = 74;
				//HASHINT i = 37235;
				//HASHINT i = 1024-o;
				HASHINT i = (1024-o) ^ r;
				//HASHINT i = o;
			//	HASHINT i = HASHCROP( o+1 );
				//HASHINT i = o * MAXJUMP;

				//i &= ((1<<1) | (1<<3) | (1<<4) | (1<<5));

				//i = (3-o)&(4-1);

				Map add;
				add.i = HASHCROP(i);
				add.o = HASHCROP(o);

				//if(o>1)
				//	add.o = HASHCROP( -1 );

#if 1
				bool have = false;

				for(auto mit=map.begin(); mit!=map.end(); mit++)
				{
					if(mit->i != add.i)
						continue;

					have = true;
					break;
				}

				if(have)
					continue;
#endif
				map.push_back(add);
				break;
			}
		}

#if 0
				Map add;
				add.i = HASHCROP(8);
				add.o = HASHCROP(0);
				map.push_back(add);
				
				add.i = HASHCROP(10);
				add.o = HASHCROP(1);
				map.push_back(add);
#endif
		//Log("start");

		auto mit = map.begin();

		//setup
		std::list<Mask>* masks = new std::list<Mask> [map.size()];
		std::list<Tried>* tried = new std::list<Tried> [map.size() * HASHLEVELS];
		std::list<Tried>* wtried = new std::list<Tried> [map.size() * HASHLEVELS];
		HASHINT mci = 0;
		
		HASHINT* inval = new HASHINT[map.size()];
		HASHINT* inmask = new HASHINT[map.size()];
		HASHINT* in = new HASHINT[map.size()];
		memset(inval, 0, sizeof(HASHINT)*map.size());
		memset(inmask, 0, sizeof(HASHINT)*map.size());
		memset(in, 0, sizeof(HASHINT)*map.size());

		//if(0)	//setup
		while(mit != map.end())
		{
			//HASHINT old[HASHLEVELS];
			//HASHINT m2[HASHLEVELS];
			//memcpy(old, m, sizeof(HASHINT) * HASHLEVELS);
			//memcpy(m2, m, sizeof(HASHINT) * HASHLEVELS);

			//Log("%u->%u", (int)mit->i, (int)mit->o);

			//std::list<Mask>* masks = ;
			
			//Log("answer2:");
			Mask mask;
			mask.ovaldown = mit->o;
			mask.mask = 0;
			masks[mci].push_back(mask);

			Up(mit->i, &inval[mci], &inmask[mci]);

			in[mci] = mit->i;

			++mci;
			++mit;
		}

	//up0 m0 i[#-1]1 i[#]0 if left 0 right 1
	//up0 m0 i[#-1]1 i[#]1 if 0 right left 1
	//up0 m1 i[#-1]0 i[#]1 if both 1 mask set both 0 impossible
	//up0 m1 i[#-1]1 i[#]0 if left 0 mask set
	//up1 m0 i[#-1]0 i[#]0 if left 1 right also
	//up1 m0 i[#-1]0 i[#]1 if left 1 mask set
	//up1 m1 i[#-1]0 i[#]0if left 01 mask set11
	//up1 m1 i[#-1]1 i[#]1if right 0 left
	//then justget o by m below

	//0000
	//0101
	//1010

	//0000
	//0000
	//1111

	//0000
	//1111
	//1010

	//0001
	//0101
	//1011

	//1000
	//0101
	//0011

	//1010
	//0101
	//0101

		{
			auto mit2 = map.begin();

			for(; mit2!=map.end(); mit2++)
			{
				HASHINT o = Hash(m, mit2->i, 0);
				Log("start %u->%u (correct: %u)", (int)mit2->i, (int)o, (int)mit2->o);
			}
		}

		{
			std::list<HASHINT>* up = new std::list<HASHINT> [map.size() * HASHLEVELS];
			
			auto mit2 = map.begin();
			auto i=0;
			for(; mit2!=map.end(); mit2++)
			{
				//UpFill(mit2->i, &up[HASHLEVELS*i]);
				++i;
			}
			delete [] up;
		}
		Log("up)");

		{

			HASHINT d = 0;

			if(Down(masks, m, m2, &d, HASHLEVELS-1, in, true, inval, inmask, map.size(), tried, 0, wtried))
			//if(Try12(m, mit->i, mit->o))
				;//InfoMess("s","s");
			else
			{
				//InfoMess("f","f");
				
				//memcpy(m, m2, sizeof(HASHINT) * HASHLEVELS);

				mit = map.begin();

				for(; mit!=map.end(); mit++)
				{
					HASHINT o = Hash(m, mit->i, 0);
					Log("fail gives %u->%u (correct: %u)", (int)mit->i, (int)o, (int)mit->o);
				}
				
			Log("answer:");

				uint64_t p = GetTicks() - t;

				char ms[123];
				sprintf(ms, "fail %llu on %d", p, (int)mit->o);
				MessageBox(NULL, ms, ms, NULL);

				delete [] masks;

				return;
			}
			
			delete [] masks;
			
			//memcpy(m, m2, sizeof(HASHINT) * HASHLEVELS);

			mit = map.begin();
			
			Log("success %u->%u", (int)mit->i, (int)mit->o);
			++mit;
			Log("success %u->%u", (int)mit->i, (int)mit->o);
			
#if 1
			
			auto mit2 = map.begin();

			for(; mit2!=map.end(); mit2++)
			{
				HASHINT o = Hash(m, mit2->i, 0);
				Log("gives %u->%u (correct: %u)", (int)mit2->i, (int)o, (int)mit2->o);
			}
			//break;
			
			for(SHASHADDR x=0; x<HASHLEVELS; ++x)
			{
				Log("\t mask[%d] = %llu", (int)x, m[x]);
			}
#endif

#if 0
			if((Ahead(old, m) || Ahead(m, old)) && mit != map.begin())
				mit = map.begin();
			else
#endif
			//	mit++;
		}

		mit = map.begin();

		for(; mit!=map.end(); mit++)
		{
			HASHINT o = Hash(m, mit->i, 0);
			Log("gives %u->%u (correct: %u)", (int)mit->i, (int)o, (int)mit->o);
		}

		//__int128 i = 123ull;

			for(SHASHADDR x=0; x<HASHLEVELS; ++x)
			{
				Log("\t mask[%d] = %llu", (int)x, m[x]);
			}
	}

	uint64_t p = GetTicks() - t;

	char ms[123];
	sprintf(ms, "pass %llu", p);
	MessageBox(NULL, ms, ms, NULL);
	//InfoMess(ms,ms);
#endif
}

#if 1

bool UniqueMask(Mask a, Mask b)
{
	return (a.ovaldown == b.ovaldown);
}

bool CompareMask(Mask a, Mask b)
{
	return ( (a.mask < b.mask) && (a.maskup == b.maskup) ) || ( a.maskup < b.maskup );
}

void Up(HASHINT in, HASHINT* inval, HASHINT* inmask)
{
	
	//up0 m0 i[#-1]1 i[#]0 if left 0 right 1
	//up0 m0 i[#-1]1 i[#]1 if 0 right left 1
	//up0 m1 i[#-1]0 i[#]1 if both 1 mask set both 0 impossible
	//up0 m1 i[#-1]1 i[#]0 if left 0 mask set
	//up1 m0 i[#-1]0 i[#]0 if left 1 right also
	//up1 m0 i[#-1]0 i[#]1 if left 1 mask set
	//up1 m1 i[#-1]0 i[#]0if left 01 mask set11
	//up1 m1 i[#-1]1 i[#]1if right 0 left
	//then justget o by m below

	HASHINT shift = HASHROTL(in,1);

	*inval |= HASHCROP( ~shift & ~in );
	*inmask |= *inval;

	*inmask |= HASHCROP( shift & ~in );
}

bool UniqueUp(HASHINT a, HASHINT b)
{
	return (a==b);
}

void UpFill(HASHINT in, std::list<HASHINT>* invals, HASHINT inval, HASHINT inmask)
{
	if(!inmask)
		Up(in, &inval, &inmask);
	
	HASHINT low0 = HASHCROP( LOWEST0( inmask ) );

	if(!low0)
	{
		for(auto uit=invals->begin(); uit!=invals->end(); ++uit)
			if(*uit == inval)
				return;

		invals->push_back(inval);
		return;
	}

	inmask |= low0;

	inval &= HASHCROP( ~low0 );
	UpFill(in, invals, inval, inmask);
	inval |= low0;
	UpFill(in, invals, inval, inmask);
}

void UpFill(HASHINT in, std::list<HASHINT> upvals[HASHLEVELS])
{
	UpFill(in, &upvals[0], 0, 0);
	//upvals[0].unique(UniqueUp);

	for(HASHINT l=1; l<HASHLEVELS; ++l)
	{
		Log("up%d %d", (int)l, (int)upvals[l-1].size());
		for(auto uit=upvals[l-1].begin(); uit!=upvals[l-1].end(); uit++)
			UpFill(*uit, &upvals[l], 0, 0);

		//upvals[l].unique(UniqueUp);
	}
}

void Narrow(std::list<Mask>* a, std::list<Mask>* b)
{
	auto ait=a->begin();
	auto bit=b->begin();

	//Log("narrow %d %d", (int)a->size(), (int)b->size());
	
	//for(auto ait2=a->begin(); ait2!=a->end(); ait2++)
	//	Log("a %d", (int)ait2->mask);
	//for(auto bit2=b->begin(); bit2!=b->end(); bit2++)
	//	Log("b %d", (int)bit2->mask);

	while(true)
	{
		HASHINT c;

		if(bit == b->end())
		{
			while(ait != a->end())
				ait = a->erase(ait);
			//goto check;
			return;
		}
		
		if(ait == a->end())
		{
			while(bit != b->end())
				bit = b->erase(bit);
			//goto check;
			return;
		}

		if(bit->mask < ait->mask)
		{
			c = ait->mask;

			while(bit != b->end() &&
				bit->mask < c)
				bit = b->erase(bit);

			if(bit != b->end() &&
				bit->mask == c)
				goto up;

			while(ait != a->end() &&
				ait->mask == c)
				ait = a->erase(ait);
		}
		else
		{
			c = bit->mask;

			while(ait != a->end() &&
				ait->mask < c)
				ait = a->erase(ait);
			
			if(ait != a->end() &&
				ait->mask == c)
				goto up;

			while(bit != b->end() &&
				bit->mask == c)
				bit = b->erase(bit);
		}

up:

		//Log("narrow %d", (int)c);
		
		while(bit != b->end() &&
			bit->mask == c)
			bit++;

		while(ait != a->end() &&
			ait->mask == c)
			ait++;
	}

check:

	;
	//if(a->size() != b->size() &&
	//	(a->size() == 0 || b->size() == 0))
	//	Log("disc");
}

bool Fill(HASHINT oval, HASHINT ovaldown, HASHINT oset, HASHADDR fullbin, HASHADDR setbin, 
		  HASHINT mlow, HASHINT mval, std::list<Mask>* mask, HASHINT mset, bool check, HASHINT in, HASHINT inmask, HASHINT high, HASHINT maskup,
		  bool lowlim, std::list<Tried>* tried, std::list<Tried>* wtried, bool addw)
{
#if 0
	Log("ovaldown%d oval%d mset%d oset%d q%d mlow%d inmask%d h%d !HASHCROP(~oset)%d !HASHCROP(~mset)%d HASHCROP(~oset)%d HASHCROP(~mset)%d ~oset%d ~mset%d MSHHASH%d (int)HASHBITS%d, (int)(1<<HASHBITS)%d, (int)((1<<HASHBITS)-1)%d", 
		(int)ovaldown, (int)oval,
		(int)mset, (int)oset, (int)(!HASHCROP(~oset) && !HASHCROP(~mset)),
		(int)mlow, (int)inmask, (int)high,
		(int)(!HASHCROP(~oset)), (int)(!HASHCROP(~mset)), (int)(!!(int)HASHCROP(~oset)), (int)(!!HASHCROP(~mset)), (int)(!!(int)(~oset)), (int)(~mset), (int)MAXHASH, (int)HASHBITS, (int)(1<<HASHBITS), (int)((1<<HASHBITS)-1));
#endif

	if( ((mset & mval) < (mset & mlow)) || 
		((inmask & oset & ovaldown) != (inmask & oset & in)) ||
		((mset & mval) > (mset & high)) )
		return false;
	
	if(!lowlim && DidTry(tried, ovaldown, oset))
		return false;
	if(!lowlim && wtried && DidTry(wtried, ovaldown, oset))
		return true;

	HASHADDR bin = fullbin%HASHBITS;
	HASHADDR backbin = (bin+HASHBITS-1)%HASHBITS;

#if 0
	Log("start l%d b%d bb%d d%d o%d l%d %d %dm", 
		(int)0, (int)bin, (int)backbin, (int)ovaldown, (int)oval, (int)mval, (int)oset, (int)mset);
#endif

	HASHINT bm = ((HASHINT)1)<<bin;
	HASHINT bbm = ((HASHINT)1)<<backbin;
	
	HASHINT shifto = HASHROTL(ovaldown,1);
	HASHINT shiftoset = HASHROTL(oset,1);
	
	//up0 m0 i[#-1]1 i[#]0 if left 0 right 1
	//up0 m0 i[#-1]1 i[#]1 if 0 right left 1
	//up0 m1 i[#-1]0 i[#]1 if both 1 mask set both 0 impossible
	//up0 m1 i[#-1]1 i[#]0 if left 0 mask set
	//up1 m0 i[#-1]0 i[#]0 if left 1 right also
	//up1 m0 i[#-1]0 i[#]1 if left 1 mask set
	//up1 m1 i[#-1]0 i[#]0if left 01 mask set11
	//up1 m1 i[#-1]1 i[#]1if right 0 left
	//then justget o by m below

	//0000
	//1111
	//0101

	//0000
	//1111
	//1010

	//0000
	//0101
	//0101

	//0000
	//0111
	//0101

	//0000
	//0011
	//1110

	////////////////////

	//0001
	//1111
	//0110

	//0001
	//0101
	//1101

	//0001
	//0111
	//1101
	
	//0000
	//0011
	//0110

	HASHINT case0 = HASHCROP( ~oval & (~shifto & bm & shiftoset) );	//0?
	HASHINT case1 = HASHCROP( ~oval & (~ovaldown & bm & oset) );	//?0
	HASHINT case2 = HASHCROP( ~oval & (shifto & ovaldown & bm & shiftoset & oset) );	//11
	HASHINT error0 = HASHCROP( ~oval & (~shifto & ~ovaldown & bm & shiftoset & oset) );
	HASHINT error1 = HASHCROP( case0 & mset & ~mval );
	HASHINT error2 = ( case2 & mset & mval );

	HASHINT case3 = ( oval & (shifto & bm & shiftoset) );	//1?
	HASHINT error3 = HASHCROP( case3 & oset & ~ovaldown );
	HASHINT error4 = HASHCROP( case3 & mset & ~mval );
	HASHINT case4 = HASHCROP( oval & (oset & shiftoset & ~shifto & ovaldown & bm ) );	//01
	HASHINT error5 = ( case4 & mset & mval );
	HASHINT case5 = ( oval & (oset & ovaldown & bm) );	//?1
	HASHINT error6 = HASHCROP( oval & (shiftoset & shifto & bm & oset & ~ovaldown) );

	HASHINT case6 = case0 | case2 | case4 | case3 | (oset & ovaldown & bm);	//?1
	HASHINT case7 = case1 | (oset & ~ovaldown & bm);	//?0
	
	HASHINT case8 = case0 | case4 | (shiftoset & ~shifto & bm);	//0?
	HASHINT case9 = ( oval & (oset & shiftoset & ovaldown & shifto & bm) );	//11
	HASHINT case10 = case1 | case9 | case3 | (shiftoset & shifto & bm);	//1?

	ovaldown |= case6;
	ovaldown &= HASHCROP( ~case7 );

	ovaldown |= HASHROTR(case10,1);
	ovaldown &= HASHCROP( ~HASHROTR(case8,1) );

	HASHINT case11 = case2 | case4 | (mset & bm & ~mval);	//case m 0
	HASHINT case12 = case0 | case9 | case3 | (mset & bm & mval);	//case m 1

	HASHINT case13 = (case10 & (case11 | case7)) | (case8 & case6 & case12) | (~oval & bm);	//up0
	HASHINT case14 = (case10 & case6 & case12) | (case8 & (case11 | case7)) | (oval & bm);	//up1

	mval |= case12;
	mval &= HASHCROP( ~case11 );
	
#if 0
	Log("mid l%d b%d bb%d c0%d c1%d c2%d 3%d 4%d 5%d 6%d 7%d 8%d 9%d 10%d 11%d 12%d 13%d 14%d e%d 1%d 2%d 3%d 4%d 5%d %d d%d o%d l%d %d %dm %d", 
		(int)0, (int)bin, (int)backbin, (int)case0, (int)case1, (int)case2, (int)case3, (int)case4, (int)case5, (int)case6, (int)case7, (int)case8, (int)case9, (int)case10, (int)case11,
		(int)case12, (int)case13, (int)case14, (int)error0, (int)error1, (int)error2, (int)error3, (int)error4, (int)error5, (int)error6, (int)ovaldown, (int)oval, (int)mval, (int)oset, (int)mset, (int)shiftoset);
#endif

	if(error0 || error1 || error2 || error3 || error4 || error5 || error6)
		return false;

	if((case11 && case12) || (case8 && case10) || (case6 && case7) | (case13 && case14))
		return false;

	if(!HASHCROP(~oset) && !HASHCROP(~mset))
	{
		Mask m;
		m.mask = mval;
		m.ovaldown = ovaldown;
		m.maskup = maskup;
		mask->push_back(m);

		//if(!lowlim)
		//	AddTried(tried, ovaldown);

		//Log("%d for %d->%d", (int)mval, (int)ovaldown, (int)oval);

		return false;
	}

	oset |= bm;
	oset |= bbm;
	mset |= bm;

#if 0
	Log("downfill l%d b%d bb%d c0%d c1%d c2%d 3%d 4%d 5%d 6%d 7%d 8%d 9%d 10%d 11%d 12%d e%d 1%d 2%d 3%d 4%d 5%d %d d%d o%d l%d %d %dm %d", 
		(int)0, (int)bin, (int)backbin, (int)case0, (int)case1, (int)case2, (int)case3, (int)case4, (int)case5, (int)case6, (int)case7, (int)case8, (int)case9, (int)case10, (int)case11,
		(int)case12, (int)error0, (int)error1, (int)error2, (int)error3, (int)error4, (int)error5, (int)error6, (int)ovaldown, (int)oval, (int)mval, (int)oset, (int)mset, (int)shiftoset);
#endif
	
	HASHADDR nextbin = backbin;
	//HASHADDR nextbin = bin+(HASHADDR)!check;

#if 0
	if((!case11 && !case12) ||
		(!case8 && !case10) ||
		(!case6 && !case7))
#endif
	if(check)
		nextbin = bin;

	check = !check;

	for(HASHINT r=0; r<2; ++r)
	{
		for(HASHINT l=0; l<2; ++l)
		{
			for(HASHINT m=0; m<2; ++m)
			{
				if(Fill(oval, ovaldown, oset, nextbin, setbin, mlow, mval, mask, mset, check, in, inmask, high, maskup, lowlim, tried, wtried) &&
					wtried)
					return true;

				if(!case11 && !case12 && !check)
					mval ^= (((HASHINT)1)<<bin);
				else
					break;
			}
		
			if(!case8 && !case10 && !check)
				ovaldown ^= (((HASHINT)1)<<backbin);
			else
				break;
		}
	
		if(!case6 && !case7 && !check)
			ovaldown ^= (((HASHINT)1)<<bin);
		else
			break;
	}

	return false;

	//choice 1
	//so i[#-1]=1 and i[#]=1 and m0[#]=1
	//or i[#-1]=0 and i[#]=0 and m0[#]=0
	//or i[#-1]=0 and i[#]=0 and m0[#]=1
	//or i[#-1]=0 and i[#]=1 and m0[#]=0

	//choice down 0
	//so o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=0 and m(level)[bin]=0
	//or o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=1 and m(level)[bin]=0
	//or o(level-1)[(bin-1)%nb]=1 and o(level-1)[bin]=0 and m(level)[bin]=1
	//or o(level-1)[(bin-1)%nb]=0 and o(level-1)[bin]=1 and m(level)[bin]=1

	//up0 m0 i[#-1]1 i[#]0 if left 0 right 1
	//up0 m0 i[#-1]1 i[#]1 if 0 right left 1
	//up0 m1 i[#-1]0 i[#]1 if both 1 mask set both 0 impossible
	//up0 m1 i[#-1]1 i[#]0 if left 0 mask set
	//up1 m0 i[#-1]0 i[#]0 if left 1 right also
	//up1 m0 i[#-1]0 i[#]1 if left 1 mask set
	//up1 m1 i[#-1]0 i[#]0if left 01 mask set11
	//up1 m1 i[#-1]1 i[#]1if right 0 left
	//then justget o by m below

	//0 0 1 0 0 1 1 1    0 1 0 1 0 1 1 1    0 0 1 0 0 1 0 1    0 1 0 1 0 1 0 1
	//0 1 1 0 1 1 1 1
	//0 1 1 1 0 0 0 0
	//0 1 1 1 0 0 0 1
	//0 1 1 1 0 0 1 1
	//0 1 1 1 1 1 1 1
	//1 0 0 1 0 0 0 0
	//1 0 0 1 0 0 0 1
	//1 0 0 1 0 0 1 1
	//1 0 0 1 0 1 1 1
	//1 0 0 1 1 1 1 1
	//1 0 1 0 1 1 1 1
	//1 0 1 1 0 0 0 0
	//1 0 1 1 0 0 0 1
	//1 0 1 1 0 0 1 1
	//1 0 1 1 0 1 1 1
	//1 0 1 1 1 1 1 1
	//1 1 1 0 1 1 1 1
	//1 1 1 1 0 0 0 0
	//1 1 1 1 0 0 0 1
	//1 1 1 1 0 0 1 1
	//1 1 1 1 0 1 1 1
	//1 1 1 1 1 1 1 1

	//0 0 1 0
	//0 1 1 1
	//1 0 0 1
	//1 0 1 0
	//1 0 1 1
	//1 1 1 0
	//1 1 1 1

	//0 1 1 1
	//0 0 0 0
	//0 0 0 1
	//0 0 1 1
	//0 1 1 1
	//1 1 1 1

	//0 1 0 1
	//0 0 1 1
	//0 1 0 0
	//0 1 0 1
	//0 1 1 1
	//1 1 0 0
	//1 1 0 1
	//1 1 1 1
}

void Merge(std::list<Tried>* t)
{
	//return;
again:
	for(auto t1=t->begin(); t1!=t->end(); t1++)
	{
		auto t2=t->begin();
		for(t2++; t2!=t->end(); t2++)
		{
			//if(t2==t1)
			//	continue;
			
			HASHINT d = (HASHCROP(~t1->mask) & t1->tried) ^ 
				(HASHCROP(~t2->mask) & t2->tried);


			HASHINT l1 = HASHCROP( LOWEST1(d) );

			if(!l1)
			{
				//0m
				//mm
				//
				//0m
				//m0
				d = t1->mask ^ t2->mask;

				if(d)
				{
					l1 = HASHCROP( LOWEST1(d) );
					d ^= l1;

					if(!d)
					{
						//one dif
						if(t1->mask < t2->mask)
							t->erase(t1);
						else
							t->erase(t2);

						goto again;
					}

					continue;
				}

				t->erase(t2);
				goto again;
			}

			d ^= l1;

			if( HASHCROP( LOWEST1(d) ) )
				continue;

			//one

			if(t1->mask != t2->mask)
				continue;

			t1->mask |= l1;
			t->erase(t2);
			
			//0m
			//m1

			//00
			//01

			//01
			//11
			
			goto again;
		}
	}
}

void AddTried(std::list<Tried>* t, HASHINT s)
{
	//return;
#if 1
	for(auto tit=t->begin(); tit!=t->end(); ++tit)
	{
		HASHINT d = (HASHCROP(~tit->mask) & tit->tried) ^ 
			(HASHCROP(~tit->mask) & s);

		HASHINT l1 = HASHCROP( LOWEST1(d) );

		//Log("t %d,%d,%d", (int)tit->tried, (int)tit->mask, (int)s);

		if(!l1)
		{
			//Log("ret %d %d m%d", (int)s, (int)tit->tried, (int)tit->mask);
			return;	//must be one or more
		}

		d ^= l1;

		if( HASHCROP( LOWEST1(d) ) )
		{
			//Log("cona %d %d m%d", (int)s, (int)tit->tried, (int)tit->mask);
			continue;	//must be single
		}

		if(!tit->mask)
			tit->mask |= l1;

		//Log("add a (s%d,m%d)(%d) merge", (int)tit->tried, (int)tit->mask,(int)s);

		goto merge;
	}
#endif
	Tried st;
	st.tried = s;
	st.mask = 0;
	t->push_back(st);

merge:

	Merge(t);
	
		//Log("add add (%d) merge", (int)s);
}

bool DidTry(std::list<Tried>* t, HASHINT s)
{
	for(auto tit=t->begin(); tit!=t->end(); ++tit)
	{
		if((HASHCROP(~tit->mask) & tit->tried) == 
			(HASHCROP(~tit->mask) & s))
			return true;
	}

	return false;
}


bool DidTry(std::list<Tried>* t, HASHINT s, HASHINT setmask)
{
	for(auto tit=t->begin(); tit!=t->end(); ++tit)
	{
		if(((HASHCROP(~tit->mask) & tit->tried) == (HASHCROP(~tit->mask) & s)) &&
			!(HASHCROP(~tit->mask) & HASHCROP(~setmask)))
		{
#if 0
			Log("true tit%d,s%d, s%d,sm%d ((%d=(%d==%d)) && (%d=!(%d&%d)))", 
				(int)tit->mask, (int)tit->tried,
				(int)s, (int)setmask,
				(int)((HASHCROP(~tit->mask) & tit->tried) == (HASHCROP(~tit->mask) & s)),
				(int)(HASHCROP(~tit->mask) & tit->tried), 
				(int)(HASHCROP(~tit->mask) & s),
				(int)!(HASHCROP(~tit->mask) & HASHCROP(~setmask)),
				(int)HASHCROP(~tit->mask),
				(int)HASHCROP(~setmask));
#endif
			return true;
		}
	}

	return false;
}


bool Limit(std::list<Mask>* masksup, 
		  HASHINT mval[HASHLEVELS], 
		  HASHINT mvalnew[HASHLEVELS], 
		  HASHINT* maskup, 
		  HASHADDR level, 
		  HASHINT* in, 
		  bool lowlim, 
		  HASHINT* upin, 
		  HASHINT* upinmask, 
		  HASHINT nmc,
		  std::list<Tried>* tried,
		  HASHADDR downmci,
		  std::list<Mask>* mu,
		  HASHINT upmask,
		  HASHADDR mci,
		  bool original,
		  std::list<Tried>* wtried)
{
	if(level == (HASHADDR)-1)
		return true;

	Log("lim lev%d ms%d", (int)level, (int)masksup->size());

	//if(lowlim)
	//	return;

	HASHINT inmask = 0;
	
	//Log("zeromask[0]", (int)zeromask[0]);
	//Log("inmask[0]", (int)inmask[0]);

	if(!level)
	{
		inmask = (HASHINT)-1;
	}

	HASHINT inval = in[mci];

	if(level==1)
	{
		inval = upin[mci];
		inmask = upinmask[mci];
	}

	HASHINT lowest = 0;
	HASHINT high = 1;
	
	std::list<Tried>* tlc = &tried[nmc*level+mci];
	std::list<Tried>* wtlc = &wtried[nmc*level+mci];

	std::list<Mask> masks;
	std::list<Mask>* m = &masks;

	//BinTree tried;
	//HASHINT tried[2] = {0,0};

	auto muit = mu->begin();

	while(muit!=mu->end() && muit->mask < upmask)
		//muit++;
		muit = mu->erase(muit);

	auto start = muit;

	bool r = false;

	while(muit!=mu->end() &&
		(!original || muit->mask == upmask))
	{
		//Log("while(muit!=mu->end())");

		upmask = muit->mask;
		HASHINT oval = muit->ovaldown;
#if 0
		if(DidTry(wtlc, oval))
		{
			r = true;
			continue;
		}
#endif

		//Log("lev%d m%d continue mask%u", (int)level, (int)masksup->size(), (int)upmask);

		const HASHADDR setbin = HASHBITS-1;
		const HASHINT ovaldown = 0;
		const HASHINT oset = 0;
		//
		//Log("inmask[mci]%d", (int)inmask[mci]);
		//Log("zeromask[mci]%d", (int)zeromask[mci]);
		//Log("inmask[mci]%d", (int)inmask[0]);
		//Log("zeromask[mci]%d", (int)zeromask[0]);
		bool skipw = Fill(oval, ovaldown, oset, setbin, setbin, mval[level], 0, m, 0, true, inval, inmask, high, upmask, false, tlc, wtlc);

		muit++;

		//m->sort(CompareMask);
		m->unique(UniqueMask);	//only for 1 ch

		//for(HASHINT mci2=0; mci2<mci; ++mci2)
		//	Narrow(m, &masks[mci2]);

		//if(submasks.size() && Down(&submasks, mval, &mval[level], level-1, in, lowlim, upin, upinmask))
		if(m->size() && Limit(&masks, mval, mvalnew, &mval[level], level-1, in, lowlim, upin, upinmask, nmc, tried, mci,
			&masks, 0, mci, false, wtried))
		{
			//Log("m from%llu to %llu", submasks.begin()->maskup, submasks.rbegin()->maskup);

			//Log("set mask[%d] = %u", (int)level, (int)mval[level]);
			//*maskup = upmask;

			//Log("success");

			//if(!original)
			if(level < HASHLEVELS-1)
			{
				std::list<Tried>* wtlcup = &wtried[nmc*(level+1)+mci];
				AddTried(wtlcup, oval);
			}

			//return true;
			r = true;
		}
		else if(!skipw);
		{
			for(auto mit=m->begin(); mit!=m->end(); ++mit)
				AddTried(tlc, mit->ovaldown);
#if 0
	if(level < HASHLEVELS-1)
	{
			if(!r)
			{
				std::list<Tried>* tlcup = &tried[nmc*(level+1)+mci];
				AddTried(tlcup, oval);
			}
	}
#endif
		}

		m->clear();

		if(muit!=mu->end() &&
			(!original || muit->mask == upmask))
			continue;

		//mval[level] = high;

		HASHINT low0 = HASHCROP( LOWEST0( high ) );
		high |= low0;

		if(low0)
		{
			muit = start;

			//Log("muit = start");

		}
	}

	return r;
}

//propagate down to gotten input
bool Down(std::list<Mask>* masksup, 
		  HASHINT mval[HASHLEVELS], 
		  HASHINT mvalnew[HASHLEVELS], 
		  HASHINT* maskup, 
		  HASHADDR level, 
		  HASHINT* in, 
		  bool lowlim, 
		  HASHINT* upin, 
		  HASHINT* upinmask, 
		  HASHINT nmc,
		  std::list<Tried>* tried,
		  HASHADDR downmci,
		  std::list<Tried>* wtried)
{
	Log("lev%d m%d mask%u to%u lowlim%d od%llu", (int)level, (int)masksup->size(), (int)masksup->begin()->mask, (int)masksup->rbegin()->mask, (int)lowlim, masksup->begin()->ovaldown);

	//if(masksup->size() != masksup[1].size())
	//	Log("disc");

	if(level == (HASHADDR)-1)
	{
		*maskup = masksup[0].begin()->mask;
		return true;
	}

	HASHINT mset = 0;
	HASHINT oval = 0;

	if(!lowlim)
		mval[level] = 0;

	HASHADDR upbin=HASHBITS-1;

	HASHINT* zeromask = new HASHINT[nmc];
	memset(zeromask, 0, sizeof(HASHINT)*nmc);

	HASHINT* inmask = zeromask;
	
	//Log("zeromask[0]", (int)zeromask[0]);
	//Log("inmask[0]", (int)inmask[0]);

	if(!level)
	{
		//inmask = (HASHINT)-1;
		memset(zeromask, -1, sizeof(HASHINT)*nmc);

		
	//Log("zeromask[0]", (int)zeromask[0]);
	//Log("inmask[0]", (int)inmask[0]);
	}

	HASHINT* inval = in;

	if(level==1)
	{
		inval = upin;
		inmask = upinmask;
	}

	
					//Log("inmask[mci]%d", (int)inmask[0]);
					//Log("zeromask[mci]%d", (int)zeromask[0]);
					//Log("in[mci]%d", (int)in[0]);

	HASHINT high = mval[level];
	HASHINT lowest = mval[level];

	if(!high)
		high = 1;

	//Log("low %llu", lowest);

	std::list<Mask>* masks = new std::list<Mask>[nmc];
	HASHINT upmask = 0;
	HASHINT mci = 0;

	while(mci<nmc)
	{
skipmc:
		//Log("skipmc");

		std::list<Mask>* mu = &masksup[mci];
		std::list<Mask>* m = &masks[mci];

		auto muit = mu->begin();

nextmu:
		//Log("nextmu");
		
		std::list<Tried>* tlc = &tried[nmc*level+mci];

		//BinTree tried;
		//HASHINT tried[2] = {0,0};

		while(muit!=mu->end() && muit->mask < upmask)
			//muit++;
			muit = mu->erase(muit);
		
		auto start = muit;

		while(muit!=mu->end())
		{
			//Log("while(muit!=mu->end())");

			upmask = muit->mask;
			HASHINT oval = muit->ovaldown;

			if(upmask > *maskup && lowlim)
			{
				mval[level] = 0;
				lowest = 0;
				high = 1;
				lowlim = false;
				//upbin = HASHBITS-1;
				//Log("upmask > *maskup && lowlim");
				
				if(!mci)
				{
					for(HASHADDR mci2=0; mci2<nmc; ++mci2)
					{
						//Limit();
						Limit(masksup, mval, mvalnew, maskup, level, in, true, upin, upinmask, nmc, tried, mci2, mu, upmask, mci2, true, wtried);
					}
				}
			}

			//Log("lev%d m%d continue mask%u", (int)level, (int)masksup->size(), (int)upmask);

			const HASHADDR setbin=upbin;
			{

				//if(!mci)
				{
					HASHINT ovaldown = 0;
					HASHINT oset = 0;
					//
					//Log("inmask[mci]%d", (int)inmask[mci]);
					//Log("zeromask[mci]%d", (int)zeromask[mci]);
					//Log("inmask[mci]%d", (int)inmask[0]);
					//Log("zeromask[mci]%d", (int)zeromask[0]);
					Fill(oval, ovaldown, oset, setbin, setbin, mval[level], 0, m, mset, true, inval[mci], inmask[mci], high, upmask, lowlim, tlc, NULL);
				}
#if 0
				else
				{
					std::list<Mask>* ncmasks = &masks[0];
					for(auto ncmit=ncmasks->begin(); ncmit!=ncmasks->end(); ncmit++)
					{
						HASHINT ovaldown = 0;
						HASHINT oset = 0;
						HASHINT mv = ncmit->mask;
						HASHINT ms = HASHCROP( ~(HASHINT)1 );
						Fill(oval, ovaldown, oset, setbin, setbin, mval[level], mv, m, ms, true, inval[mci], inmask[mci], high, upmask, lowlim, tlc);
					}
				}
#endif
			}

			muit++;

			if(muit == mu->end() || muit->mask > upmask)
			{
				m->sort(CompareMask);
				//m->unique(UniqueMask);	//only for 1 ch

				for(HASHINT mci2=0; mci2<mci; ++mci2)
					Narrow(m, &masks[mci2]);

				//Log("muit == mu->end() || muit->mask > upmask");

#if 0
				//HASHINT maskdown;

				std::list<Mask> submasks;

				auto smit=masks.rbegin();

				while(smit!=masks.rend() && smit->maskup >= upmask)
					smit++;

				//smit--;
				//auto fsmit = --(smit.base());
				auto fsmit = (smit.base());

				while(fsmit!=masks.end())
				{
					//Log("add upmask %llu %llu %llu", fsmit->maskup, fsmit->mask, fsmit->ovaldown);
					submasks.push_back(*fsmit);
					fsmit++;
				}
#endif

#if 0
				for(auto cit=submasks.begin(); cit!=submasks.end(); cit++)
				{
					Log("(%u) %u->%u->%u", (int)cit->maskup, (int)cit->ovaldown, (int)cit->mask, (int)oval);
				}
				Log("e");
#endif
				//Log("up %u", (int)upmask);
				//Log("m%d", (int)submasks.size());

				if(mci == nmc-1)
				{
					//Log("nmc-1");

					//if(submasks.size() && Down(&submasks, mval, &mval[level], level-1, in, lowlim, upin, upinmask))
					if(m->size() && Down(masks, mval, mvalnew, &mval[level], level-1, in, lowlim, upin, upinmask, nmc, tried, mci, wtried))
					{
						//Log("m from%llu to %llu", submasks.begin()->maskup, submasks.rbegin()->maskup);

						//Log("set mask[%d] = %u", (int)level, (int)mval[level]);
						*maskup = upmask;

						for(HASHINT mci2=0; mci2<nmc; ++mci2)
						{
							
							std::list<Mask>* mu2 = &masksup[mci2];

							for(muit=mu2->begin(); muit!=mu2->end() && muit->mask<upmask; ++muit)
								;
#if 0
							if(level>0)
							Log("lev%d tried(%dup)=%d ch%dm%d", 
							(int)level, (int)muit->ovaldown, 
							(int)DidTry(&tried[nmc*(level-1)+mci], muit->ovaldown,
							(HASHINT)-1),
							(int)mci,
							(int)muit->mask);
							else
								Log("top");
#endif
						}

						delete [] masks;
						delete [] zeromask;
						
						//Log("success");

						return true;
					}
#if 0
					else if(m->size())
						Log("upmask%d lev%d end%d high%d", 
						(int)upmask, (int)level, (int)(muit==mu->end()),
						(int)high);
					
					if(!lowlim)
					{
						for(HASHINT mci2=0; mci2<nmc; ++mci2)
						{
							std::list<Mask>* mu2 = &masks[mci2];
							std::list<Tried>* tlc2 = &tried[nmc*level+mci2];
							for(auto mit=mu2->begin(); mit!=mu2->end(); mit++)
							{
								AddTried(tlc2, mit->ovaldown);
							}
						}
					}
#endif

					for(; mci!=(HASHINT)-1; --mci)
						masks[mci].clear();

					mval[level] = high;

					HASHINT low0 = HASHCROP( LOWEST0( high ) );
					high |= low0;

					if(low0)
					{
						muit = start;

						//Log("muit = start");

					}
					else
					{
						if(muit == mu->end())
						{
							Log("low0 fail lev%d", (int)level);
							goto fail;
						}

						//Log("low0 else lev%d low0=%d muit->mask%d, upmask%d", 
						//	(int)level, (int)low0, (int)muit->mask, (int)upmask);

						start = muit;
						high = lowest;

						if(!high)
							high = 1;

						//lowlim = false;
						mval[level] = lowest;
						upmask = muit->mask;
					}

					mci = 0;
					goto skipmc;
				}
				else
				{
					muit = start;
					//Log("muit = start;");
				}

				goto nextmc;
			}
		}

nextmc:
		++mci;
		//Log("nextmc");
	}

fail:

	delete [] masks;
	delete [] zeromask;

	return false;
}

HASHINT Hash(HASHINT mval[HASHLEVELS], HASHINT oval, HASHADDR level)
{
	if(level >= HASHLEVELS)
		return oval;

	HASHINT shifto = HASHROTL(oval,1);
	const HASHINT ovalup = HASHCROP( shifto ^ ~(oval & mval[level]) );

	Log("level%d up%u o%u os%u", (int)level, (int)ovalup, (int)oval, (int)shifto);

	return Hash(mval, ovalup, level+1);
}

#endif